#ifndef __MAIN_H__
#define __MAIN_H__

#define DEFAULT_PORT 31337
#define S(str) _S(str)
#define _S(str) #str

typedef struct {
	char **args;
	uint16_t port;
	int fd;
} options_t;

#endif /* __MAIN_H__ */
