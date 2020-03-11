#include "debug.h"
#include <stdio.h>
#include <string.h>

#ifdef DEBUG

static const unsigned char hextable[]="0123456789abcdef";

void hexdump(void *data, size_t len)
{
	char hex[77];
	unsigned ch;

	size_t i = 0, total, hexoffset, choffset;

	while (i < len) {
		hexoffset = 19;
		choffset = 60;

		sprintf(hex, "%016lx:", (unsigned long)((char *)data + i));
		memset(hex+17, ' ', 43);

		total = i + 16;
		if (total > len) {
			total = len;
		}

		while (i < total) {
			ch = ((unsigned char *)data)[i];
			hex[choffset++] = (ch > ' ' && ch <= '~') ? ch : '.';

			hex[hexoffset++] = hextable[ch / 16];
			hex[hexoffset++] = hextable[ch % 16];

			if(i % 2) hexoffset++;

			i++;
		}

		hex[choffset] = 0x0;

		printf("%s\r\n", hex);
	}

	printf("%016lx\r\n", (unsigned long)((char *)data + i));
}

#endif /* DEBUG */
