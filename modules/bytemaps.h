#ifndef _BYTEMAPS_H
#define _BYTEMAPS_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

#include "configuration.h"
#include "inih/ini.h"
#include "dbg.h"
#include "util.h"

size_t hex2buf(char *, unsigned char **);
bool validate_hexstring(char *);
char * normalize_hexstring(char *);
unsigned char * nhex2int(char *);

char *get_display_hash(unsigned char *, size_t, composedmap_type *);
char *buf2map(unsigned char *, size_t , const char *, const char *, char ***, size_t);

#endif
