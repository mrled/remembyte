#include "util.h"

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


/* Take a path that might contain ., .., extraneous /, or a ~ character, and 
 * return the absolute path that the user intends
 */
char * resolve_path(const char *path)
{
  char *resolved=NULL, *query=NULL, *home=NULL;
  size_t path_len=0, home_len=0;

  path_len = strlen(path);
  query = malloc(path_len +1);
  check_mem(query);
  memcpy(query, path, path_len +1);
  log_debug("Query '%s'", query);

  // Check for ~
  if (path[0] == '~') {
    home = getenv("HOME");
    check(home, "Could not find HOME variable");
    home_len = strlen(home);

    if (path_len == 1) {
      // We were passed just a tilde and nothing else
      query = strdup(home);
    }
    else if ((path[1] == '/') || (path[1] == '\\')) {
      // We were passed a tilde and then a path seperator e.g. ~/file.txt
      realloc(query, home_len + path_len -1 +1); 
      check_mem(query);
      memcpy(query, home, home_len +1);
      memcpy(query +strlen(home), &path[1], path_len -1 +1);
    }
    else {
      // We were passed a tilde and then something that wasn't a path separator
      // such as "~username/file.txt"
      sentinel("Error for input: '~username' syntax is currently unsupported");
    }
  }

  resolved = realpath(query, NULL);
  if (!resolved) {
    log_debug("Could not resolve path for query '%s'", path);
  }
  free(query);
  return resolved;

error:
  free(query);
  free(resolved);
  return NULL;
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

