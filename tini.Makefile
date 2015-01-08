CCDEBUGARGS=-g -O0
CCINCLUDES=-I${HOME}/opt/homebrew/include
CCLIBS=-L${HOME}/opt/homebrew/lib 

tini: tini.c inih/ini.c util.c util.h bytemaps.h bytemaps.h 
	clang $(CCDEBUGARGS) -lm $(CCINCLUDES) $(CCLIBS) -o tini tini.c inih/ini.c util.c bytemaps.c

clean: 
	rm -f tini
