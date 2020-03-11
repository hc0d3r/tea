#ifndef __NET_H__
#define __NET_H__

#include <stdint.h>

int wait_connection(int sockfd, char *ipv4, uint16_t *port);
int start_listen(uint16_t port);
int issock(int fd);

#endif
