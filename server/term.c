#include <termios.h>
#include <unistd.h>
#include <err.h>

void stdin_raw_mode(void)
{
	struct termios tios;

	if (tcgetattr(STDIN_FILENO, &tios) == -1)
		err(1, "tcgetattr(%d, %p)", STDIN_FILENO, &tios);

	tios.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	tios.c_oflag &= ~(OPOST);
	tios.c_cflag |= (CS8);
	tios.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	tios.c_cc[VMIN] = 1;
	tios.c_cc[VTIME] = 1;

	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &tios) == -1)
		err(1, "tcsetattr(%d, TCSAFLUSH, %p)", STDIN_FILENO, &tios);
}
