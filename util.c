#include "util.h"

void dbgprintf(const char *format, ...) {
  if (DEBUGMODE) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
  }
}

// Why the fuck don't these exist in the standard
bool safe_strncmp(const char * str1, const char * str2, size_t len) {
  if ((strlen(str1) >= len) &&
    (strlen(str2) >= len) &&
    (strncmp(str1, str2, len) == 0))
  {
    return true;
  }
  else {
    return false;
  }
}
bool safe_strcmp(const char * str1, const char * str2) {
  if (strlen(str1) == strlen(str2)) {
    return safe_strncmp(str1, str2, strlen(str1));
  }
  else {
    return false;
  }
}
