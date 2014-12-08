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
