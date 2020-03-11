#include <fcntl.h>
#include <unistd.h>

#include "tas/tas.h"

#include "upload.h"
#include "debug.h"

void uploadme(int master)
{
	char buf[10000];

	ssize_t n;
	int fd;

	fd = open(current_exe, O_RDONLY);
	if (fd == -1) {
		debug("failed to open current_exe (%s)", current_exe);
		return;
	}

	while ((n = read(fd, buf, sizeof(buf))) > 0) {
		(void) write(master, buf, n);
	}

	close(fd);
}
