#ifndef _UTIL_H
#define _UTIL_H

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

/* Detect what OS we're on so I can deal with cross compatible stuff later.
 * It is my hope that I can keep all OS-dependent code in the util.
 * Note that POSIX is a subtype of UNIX in this code (is there even a Unix that
 doesn't do Posix? 
 */
#if !defined(_WIN32) && (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)))
  #define REMEMBYTE_OS_UNIX
  #include <unistd.h>
  #if defined(_POSIX_VERSION)
    #define REMEMBYTE_OS_POSIX
  #endif
#elif defined(WIN32)
  #define REMEMBYTE_OS_WINDOWS  
#else 
  #error Remembyte build system cannot detect operating system
#endif

extern bool DEBUGMODE;

void dbgprintf(const char *, ...);
bool safe_strcmp(const char *, const char *);
bool safe_strncmp(const char *, const char *, size_t);
char * resolve_path(const char *);
bool str2bool(char *);
char *safe_strdup(char *);

#endif
