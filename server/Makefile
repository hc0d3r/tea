CFLAGS+=-Wall -Wextra -O3

server: net.o replace.o term.o main.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c %.h
main.o: net.o replace.o term.o

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

clean:
	rm -f server net.o replace.o term.o main.o
