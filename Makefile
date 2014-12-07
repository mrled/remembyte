# remembyte Makefile
# 
# Override variables by creating a new makefile, setting your variables, and 
# including this Makefile. 
#
# For example, you might create a new makefile called new_Makefile.txt and 
# saving the following: 
#
#     CCINCLUDES=-I${HOME}/opt/homebrew/include -L${HOME}/opt/homebrew/lib
#     include Makefile
#
# Now invoke make with the `-f` flag and tell it to use your new makefile: 
#
#     make -f ./new_Makefile.txt
#

ifndef CC
CC=clang
endif

ifndef CFLAGS
#CFLAGS=-Wall
CFLAGS=
endif

ifndef CCARGS
CCARGS=
endif

# -g tells clang to generate complete debugging info
remembyte: remembyte.c remembyte.h bytemaps.c bytemaps.h act_ssh.c act_ssh.h
	$(CC) -g -O0 $(CCARGS) -lssh -lm -o remembyte remembyte.c bytemaps.c act_ssh.c

clean:
	rm -f remembyte
	rm -rf remembyte.dSYM
