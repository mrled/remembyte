CC=clang
#CFLAGS?=-Wall
#CCINCLUDES=?
#CCLIBS=?
CCDEBUGARGS?=-g -O0
LINKER=-lssh -lm

MODULESH=$(wildcard modules/*.h modules/**/*.h)
MODULESC=$(wildcard modules/*.c modules/**/*.c)
$(warning Found these module source files: $(MODULESH) $(MODULESC))

MAINSC=$(wildcard mains/*.c)
MAINSX=$(patsubst mains/%.c,%,$(MAINSC))
$(warning Found these main source files: $(MAINSC))
$(warning Will compile to these main binaries: $(MAINSX))

TESTSC=$(wildcard tests/*.c)
TESTSX=$(patsubst tests/%.c,%,$(TESTSC))
$(warning Found these test source files: $(TESTSC))
$(warning Will compile to these test binaries: $(TESTSX))

$(warning CC: $(CC))
$(warning CCDEBUGARGS: $(CCDEBUGARGS))
$(warning CFLAGS: $(CFLAGS))
$(warning CCINCLUDES: $(CCINCLUDES))
$(warning CCLIBS: $(CCLIBS))

all: bin mains

mains: bin $(MAINSX)
$(MAINSX): $(MAINSC) $(MODULESC) $(MODULESH)
	$(CC) $(CCDEBUGARGS) $(CFLAGS) $(LINKER) $(CCINCLUDES) $(CCLIBS) $(MODULESC) mains/$@.c -o bin/$@

tests: bin $(TESTSX)
$(TESTSX): 
	$(CC) $(CCDEBUGARGS) $(CFLAGS) $(LINKER) $(CCINCLUDES) $(CCLIBS) $(MODULESC) tests/$@.c -o bin/$@
	bin/$@

bin: 
	mkdir -p bin

clean:
	rm -rf bin

