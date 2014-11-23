CC=clang
#CFLAGS="-Wall"

# -g tells clang to generate complete debugging info
emossh: emossh.c bytemaps.c bytemaps.h util.c util.h
	$(CC) -g -lssh -lm -o emossh emossh.c bytemaps.c util.c

clean:
	rm -f emossh
