#include "util.h"

int DEBUGMODE;

void dbgprintf(const char *format, ...) {
  if (DEBUGMODE) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
  }
}

// Why the fuck doesn't this exist in the standard
bool safe_strcmp(const char * str1, const char * str2) {
  if ((strlen(str1) != strlen(str2)) || (strncmp(str1, str2, strlen(str1)) != 0)) {
    dbgprintf("safe_strcmp(): '%s' != '%s'\n", str1, str2);
    return false;
  }
  else {
    dbgprintf("safe_strcmp(): '%s' == '%s'\n", str1, str2);
    return true;
  }
}