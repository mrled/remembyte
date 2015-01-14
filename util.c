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


/* Cross-platform-ly find a user's home directory
 * 
 * TODO: ideally, on Windows, resolve_path() would find a file in either 
 *       %HOME% _or_ %USERPROFILE%, but if the former exists, the latter will 
 *       not be checked at all.
 */
char *get_home_directory() {
  char *homepath = NULL;
#if REMEMBYTE_OS == REMEMBYTE_OS_UNIX
  homepath = getenv("HOME");
#elif REMEMBYTE_OS == REMEMBYTE_OS_WINDOWS
  size_t homepath_sz;
  _dupenv_s(&homepath, &homepath_sz, "HOME");
  if (!homepath) {
    _dupenv_s(&homepath, &homepath_sz, "USERPROFILE");
  }
#endif 
  return homepath;
}

/* Of course the fucking standard doesn't have a cross-platform way to resolve
 * a fucking path
 * 
 * Take a path that might contain ., .., extraneous /, or a ~ character, and 
 * return the absolute path that the user intends
 */
char * resolve_path(const char * path)
{
  char *resolved, *query;

  query = safe_strdup(path);

  // Check for ~
  if (path[0] == '~') {
    char *home = get_home_directory();

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

#if REMEMBYTE_OS == REMEMBYTE_OS_POSIX
  resolved = realpath(query, NULL);
#elif REMEMBYTE_OS == REMEMBYTE_OS_WINDOWS
  // TODO: This means the max path length on Windows that we can deal with is
  //       260 chars. Should use newer APIs to deal with longer paths here.
  resolved = malloc(_MAX_PATH);
  _fullpath(resolved, query, _MAX_PATH);
#endif

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

/* Copy a string
 * 
 * strdup() is a POSIX thing, apparently
 */
char *safe_strdup(char *string) {
  char *string_copy;
  int buffer_sz = strlen(string) +1; // +1 for \0
  string_copy = malloc(buffer_sz);
  memcpy(string_copy, string, buffer_sz);
  return string_copy;
}