#ifndef _UTIL_H
#define _UTIL_H

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

void dbgprintf(const char *, ...);
bool safe_strcmp(const char *, const char *);

#endif
