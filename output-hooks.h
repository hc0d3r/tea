#ifndef __OUTPUT_HOOKS_H__
#define __OUTPUT_HOOKS_H__

#include <stdio.h>

#define MAGIC "s1ght-b3y0nd-s1ght"

void wait_last_login(void *ptr, char **chunk, size_t *bufsize);
void wait_magic(void *ptr, char **chunk, size_t *bufsize);
void wait_answer(void *ptr, char **chunk, size_t *bufsize);

#endif
