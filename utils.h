#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>

int match(const char *data, int len, const char *search, int *pos);
int getfilesize(const char *filename);
char *gethome(void);

#endif
