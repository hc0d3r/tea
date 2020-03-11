#ifndef __DEBUG_H__
#define __DEBUG_H__

#ifdef DEBUG
#include <stdio.h>

#define debug(fmt, vargs...) do { \
	fprintf(stderr, "(" __FILE__ ":%d) %s: " fmt "\r\n", __LINE__, __func__, ##vargs); \
} while(0)

void hexdump(void *data, size_t len);

#else
#define hexdump(x...)
#define debug(x...)
#endif

#endif
