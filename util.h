#ifndef _UTIL_H
#define _UTIL_H

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "dbg.h"

bool safe_strcmp(const char *, const char *);
bool safe_strncmp(const char *, const char *, size_t);
char * resolve_path(const char *);
bool str2bool(char *);

#endif
