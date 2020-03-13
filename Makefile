CFLAGS+=-Wall -Wextra -Itas
LDFLAGS+=tas/.obj/libtas.a -static -lutil

OBJS = 	bin/install.o bin/inject-cmd.o bin/output-hooks.o \
		bin/post-install.o bin/upload.o bin/utils.o bin/debug.o

ifneq ($(IP),)
	REMOTE_TARGET+=-DIP='"$(IP)"'
endif

ifneq ($(PORT),)
	REMOTE_TARGET+=-DPORT=$(PORT)
endif

.PHONY: all clean

all: tas/.obj/libtas.a ssh

ssh: ssh.c $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	strip $@

# __dependencies__

bin/inject-cmd.o: bin/output-hooks.o bin/utils.o debug.h
bin/install.o: bin/utils.o debug.h
bin/output-hooks.c: bin/inject-cmd.o bin/upload.o bin/utils.o bin/debug.o
bin/upload.o: debug.h
bin/post-install.o: CFLAGS+=$(REMOTE_TARGET)

bin/%.o: %.c %.h

# __end__

tas/.obj/libtas.a:
	cd tas; $(MAKE)

bin/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f ssh $(OBJS)
