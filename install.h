#ifndef __INSTALL_H__
#define __INSTALL_H__

enum {
	BASHRC_INFECTED,
	BASHRC_NOT_INFECTED,
	BASHRC_NOT_EXISTS,
};

int check_bashrc(const char *bashrc);
int infect_bashrc(const char *bashrc);

#endif
