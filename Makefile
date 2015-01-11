# remembyte Makefile
# 
# Override variables by creating a new makefile, setting your variables, and 
# including this Makefile. 
#
# For example, you might create a new makefile called new_Makefile.txt and 
# saving the following: 
#
#     CCINCLUDES=-I${HOME}/opt/homebrew/include 
#     CCLIBS=-L${HOME}/opt/homebrew/lib
#     include Makefile
#
# Now invoke make with the `-f` flag and tell it to use your new makefile: 
#
#     make -f ./new_Makefile.txt
#

# TODO: when I use CC as the variable name here, my Linux machine appears to 
#       use /bin/cc even if AFAICT $CC is not defined in my shell. I use 
#       $CCOMPILER instead to hack around that, but I wonder if there's a better
#       way.
ifndef CCOMPILER
CCOMPILER=clang
endif

ifndef CFLAGS
#CFLAGS=-Wall
CFLAGS=
endif

ifndef CCINCLUDES
CCINCLUDES=
endif

ifndef CCLIBS
CCLIBS=
endif

ifndef CCDEBUGARGS
CCDEBUGARGS=-g -O0
endif

remembyte: remembyte.c bytemaps.c bytemaps.h act_ssh.c act_ssh.h util.c util.h inih/ini.c inih/ini.h
	$(CCOMPILER) $(CCDEBUGARGS) -lssh -lm $(CCINCLUDES) $(CCLIBS) remembyte.c bytemaps.c act_ssh.c util.c inih/ini.c -o remembyte 

clean:
	rm -f remembyte
	rm -rf remembyte.dSYM
