#include "net.h"
#include <unistd.h>
#include <err.h>

#include <sys/stat.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>

int start_listen(uint16_t port)
{
	static struct sockaddr_in server_addr = {
		.sin_family = AF_INET,
		.sin_addr.s_addr = 0x00000000
	};

	static const int enable = 1;

	int sockfd;

	sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockfd == -1) {
		return -1;
	}

	server_addr.sin_port = htons(port);

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) == -1) {
		close(sockfd);
		return -1;
	}

	if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
		close(sockfd);
		return -1;
	}

	if (listen(sockfd, SOMAXCONN) == -1) {
		close(sockfd);
		return -1;
	}

	return sockfd;
}

int wait_connection(int sockfd, char *ipv4, uint16_t *port)
{
	struct sockaddr_in addr;
	socklen_t len = sizeof(addr);

	int res = accept(sockfd, (struct sockaddr *)&addr, &len);
	if (res == -1) {
		goto end;
	}


	inet_ntop(AF_INET, &addr.sin_addr, ipv4, 16);
	*port = htons(addr.sin_port);

end:
	return res;
}

int issock(int fd)
{
	struct stat st;
	if (fstat(fd, &st) == -1) {
		//err(1, "fstat(%d, %p)", fd, &st);
		return 0;
	}

	return S_ISSOCK(st.st_mode);
}
