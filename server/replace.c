#include "replace.h"

#include <string.h>
#include <stdlib.h>

char *replace(const char *string, const char *oldstr, const char *newstr)
{
	char *str = NULL, *aux, *ptr;
	size_t newlen, oldlen, len, i, msize;

	if (!oldstr[0]) {
		str = strdup(string);
		goto end;
	}

	i = msize = 0;
	newlen = strlen(newstr);
	oldlen = strlen(oldstr);

	while (1) {
		if ((aux = strstr(string, oldstr))) {
			len = aux - string;
			msize += len + newlen;
			if (!msize) goto empty_size;
		} else {
			len = strlen(string) + 1;
			msize += len;
		}

		ptr = realloc(str, msize * sizeof(char));
		if (ptr == NULL) {
			free(str);
			str = NULL;
			goto end;
		}

		str = ptr;

		if (len) {
			memcpy(str + i, string, len);

			if (!aux) goto end;

			i += len;
		}

		if (newlen) {
			memcpy(str + i, newstr, newlen);
			i += newlen;
		}

empty_size:
		string = aux + oldlen;
	}

end:
	return str;
}
