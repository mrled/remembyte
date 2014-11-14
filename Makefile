CC=gcc
#CLAGS="-Wall"

emossh: emossh.c
	$(CC) -o emossh emossh.c  -lssh

clean:
	rm -f emossh
