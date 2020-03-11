#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <err.h>

#include <sys/poll.h>

#include "main.h"
#include "term.h"
#include "replace.h"
#include "net.h"

void help(void)
{
	static const char msg[]=
		"Usage: server [options] -- [cmd]\n\n"
		"Options:\n"
		" -p [port] (Default: " S(DEFAULT_PORT) ")\n"
		" -f [fd]   connection file descriptor\n"
		" -h        display this help menu\n"
		"\n"
		"Cmd - command to be executed when a new connection is created.\n"
		"You can use these strings that will be replaced by the following information:\n"
		"\n"
		" %addr% -> remote IP address\n"
		" %port% -> remote source port\n"
		" %exe%  -> current executable filename\n"
		" %fd%   -> connection file descriptor (REQUIRED)\n"
		"\n"
		"Examples:\n"
		"\n"
		"./server -p 1337 -- xterm -fa 'Monospace' -fs 10 -bg black -fg white -T '%addr%:%port%' -e %exe% -f %fd%\n"
	;

	puts(msg);
	exit(0);
}

int parse_args(options_t *opts, int argc, char **argv)
{
	int opt, ret = 1;

	while ((opt = getopt(argc, argv, "hp:f:")) != -1) {
		switch (opt) {
			case 'p':
				opts->port = atoi(optarg);
				break;
			case 'f':
				opts->fd = atoi(optarg);
				ret = 0;
				goto end;
			case 'h':
				help();
		}
	}

	argv += optind;

	opts->args = argv;
	if (*opts->args == NULL) {
		printf("missing cmd line\n");
		goto end;
	}

	while (*++argv) {
		if (strstr(*argv, "%fd%")) {
			ret = 0;
			goto end;
		}
	}

	printf("missing a parameter that contains %%fd%%\n");

end:
	return ret;
}

char **build_argv(char **args, int fd, char *ip, uint16_t port)
{
	static const char *vars[] = {
		"%fd%", "%port%", "%addr%", NULL
	};

	char strport[16], strfd[32], **ret, *newstr, *aux, *values[3];
	int i, j;

	sprintf(strport, "%" PRIu16, port);
	sprintf(strfd, "%d", fd);

	values[0] = strfd;
	values[1] = strport;
	values[2] = ip;

	for (i = 0; args[i]; i++) {}

	ret = malloc(sizeof(char *) * (i + 1));
	if (ret == NULL) {
		err(1, "malloc()");
	}

	ret[i] = NULL;

	for (i = 0; args[i]; i++) {
		aux = args[i];

		for (j = 0; vars[j]; j++) {
			newstr = replace(aux, vars[j], values[j]);
			if (newstr == NULL) {
				err(1, "replace(%p, %p, %p)", aux, vars[j], values[j]);
			}

			free(aux);
			aux = newstr;
		}

		ret[i] = newstr;
	}

	return ret;
}

int connection_handler(int fd)
{
	struct pollfd pfds[2];
	char ch, buf[4096];
	int wfd[2];

	if (!issock(fd)) {
		printf("%d is an invalid socket fd\n", fd);
		getchar();
		return 1;
	}

	stdin_raw_mode();

	pfds[0].fd = STDIN_FILENO;
	pfds[1].fd = fd;

	pfds[0].events = POLLIN;
	pfds[1].events = POLLIN;

	wfd[0] = fd;
	wfd[1] = STDOUT_FILENO;

	while (poll(pfds, 2, -1) != -1) {
		for (int i = 0; i < 2; i++) {
			if (pfds[i].revents & POLLIN) {
				ssize_t n = read(pfds[i].fd, buf, sizeof(buf));
				if (n <= 0)
					goto lost_connection;

				write(wfd[i], buf, n);
			}
		}
	}

lost_connection:
	// clear screen
	printf("\e[H\e[2J");
	printf("connection lost, press 'q' to exit...\r\n");
	while ((read(STDIN_FILENO, &ch, 1) == 1) && ch != 'q') {}

	return 0;
}

int init_server(options_t *opts)
{
	int sockfd, clifd;
	char ip[16], *exe, **argv;
	uint16_t port;

	printf("waiting for connections on port %d\n", opts->port);
	sockfd = start_listen(opts->port);

	if (sockfd == -1) {
		err(1, "start_listen(%d)", opts->port);
	}

	exe = realpath("/proc/self/exe", NULL);
	if (exe == NULL) {
		err(1, "realpath(\"/proc/self/exe\", NULL)");
	}

	for (int i = 0; opts->args[i]; i++) {
		opts->args[i] = replace(opts->args[i], "%exe%", exe);
	}

	free(exe);

	while (1) {
		clifd = wait_connection(sockfd, ip, &port);
		if (clifd == -1) {
			break;
		}

		printf("new connection from %s:%d\n", ip, port);

		pid_t pid = fork();
		switch (pid) {
			case 0:
				argv = build_argv(opts->args, clifd, ip, port);
				pid = getpid();

				for (int i = 0; argv[i]; i++) {
					printf("child(%d): argv[%d] = %s\n", pid, i, argv[i]);
				}

				execvp(argv[0], argv);
				err(1, "child(%d): execv()", pid);
			case -1:
				err(1, "fork()");
		}

		close(clifd);
	}

	return 0;
}

int main(int argc, char **argv)
{
	options_t opts = {
		.fd = -1,
		.args = NULL,
		.port = DEFAULT_PORT
	};

	if (argc < 2) {
		help();
	}

	if (parse_args(&opts, argc, argv)) {
		return 1;
	}

	if (opts.fd != -1) {
		return connection_handler(opts.fd);
	}

	return init_server(&opts);
}
