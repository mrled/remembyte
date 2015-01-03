tini: tini.c inih/ini.c util.c util.h
	clang -g -O0 -lm -I${HOME}/opt/homebrew/include -L${HOME}/opt/homebrew/lib -o tini tini.c inih/ini.c util.c

clean: 
	rm -f tini
