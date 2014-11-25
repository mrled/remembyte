CC=clang
#CFLAGS="-Wall"

# -g tells clang to generate complete debugging info
emossh: emossh.c emossh.h bytemaps.c bytemaps.h util.c util.h act_ssh.c act_ssh.h
	$(CC) -g -lssh -lm -o emossh emossh.c bytemaps.c util.c act_ssh.c

clean:
	rm -f emossh
