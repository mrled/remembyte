CC=clang
#CFLAGS="-Wall"
CFLAGS="-lm"

# -g tells clang to generate complete debugging info
emossh: emossh.c
	$(CC) -g $(CFLAGS) -o emossh emossh.c  -lssh

clean:
	rm -f emossh
