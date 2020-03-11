#include "utils.h"
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>

int getfilesize(const char *filename)
{
	struct stat st;
	if (stat(filename, &st) == -1) {
		return -1;
	}

	return (int)st.st_size;
}

int match(const char *data, int len, const char *search, int *pos)
{
	int j = *pos, found = 0;

	for (int i = 0; i < len; i++) {

check:
		if (data[i] == search[j]) {
			j++;

			if (search[j] == 0x0) {
				found = i + 1;
				break;
			}
		} else if (j) {
			j = 0;
			goto check;
		} else {
			j = 0;
		}
	}

	*pos = j;
	return found;
}

char *gethome(void)
{
	char *home = getenv("HOME");
	if (home) return home;

	struct passwd *pw = getpwuid(getuid());
	if (pw && pw->pw_dir) return pw->pw_dir;

	return ".";
}

/* I'll use this someday */
#if 0
void hexencode(char *buf, const char *str, int len)
{
	int i = 0;

	while (len > i) {
		unsigned char ch = (unsigned char)str[i];

		*buf++ = '\\';
		*buf++ = 'x';
		*buf++ = "0123456789abcdef"[ch / 16];
		*buf++ = "0123456789abcdef"[ch % 16];

		i++;
	}
}
#endif
