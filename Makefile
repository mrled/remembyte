CC=clang
#CFLAGS="-Wall"

# -g tells clang to generate complete debugging info
remembyte: remembyte.c remembyte.h bytemaps.c bytemaps.h util.c util.h act_ssh.c act_ssh.h
	$(CC) -g -O0 -lssh -lm -o remembyte remembyte.c bytemaps.c util.c act_ssh.c

clean:
	rm -f remembyte
	rm -f remembyte.dSYM
