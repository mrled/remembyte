#include "util.h"

void dbgprintf(const char *format, ...) {
  if (DEBUGMODE) {
    fprintf(stderr, "DBG: ");

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


/* Of course the fucking standard doesn't have a cross-platform way to resolve
 * a fucking path
 * 
 * Take a path that might contain ., .., extraneous /, or a ~ character, and 
 * return the absolute path that the user intends
 */
char * resolve_path(const char * path) {
  char *resolved, *query;

  query = strdup(path);

  // Check for ~
  if (path[0] == '~') {
    char *home;
    home = getenv("HOME");
    if (!home) {
      fprintf(stderr, "Could not find HOME variable\n");
      return NULL;
    }

    if (strlen(path) == 1) {
      // We were passed just a tilde and nothing else
      return home;
    }
    else if ((path[1] == '/') || (path[1] == '\\')) {
      // We were passed a tilde and then a path seperator 
      // such as "~/file.txt"
      query = strcat(home, &path[1]);
    }
    else {
      // We were passed a tilde and then something that wasn't a path separator
      // such as "~username/file.txt"
      fprintf(stderr, "Error for input '%s': '~username' syntax is currently "
        "unsupported\n", path);
      return NULL;
    }
  }

  // TODO: this isn't cross platform yet
  resolved = realpath(query, NULL);

  return resolved;
}

bool str2bool(char *str) {
  long strl;
  strl = strtol(str, NULL, 10);
  if (safe_strcmp(str, "true") || safe_strcmp(str, "True") || strl) {
    return true;
  }
  else {
    return false;
  }
}