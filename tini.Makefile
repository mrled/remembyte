CCDEBUGARGS=-g -O0
CCINCLUDES=-I${HOME}/opt/homebrew/include
CCLIBS=-L${HOME}/opt/homebrew/lib 

tini: tini.c inih/ini.c inih/ini.h util.c util.h bytemaps.c bytemaps.h 
	clang $(CCDEBUGARGS) -lm $(CCINCLUDES) $(CCLIBS) -o tini tini.c inih/ini.c util.c bytemaps.c

clean: 
	rm -fr tini tini.dSYM
