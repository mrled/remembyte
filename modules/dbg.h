/* Contains code from dbg.h by zed shaw, taken from
 * <http://c.learncodethehardway.org/book/ex20.html>
 */ 

#ifndef __dbg_h__
#define __dbg_h__

#include <stdio.h>
#include <errno.h>
#include <string.h>

#ifdef NDEBUG
#  define log_debug(M, ...)
#  define log_arguments_debug(C, V)
#else
#  define log_debug(M, ...) fprintf(stderr, "[DEBUG] %s:%d:%s(): " M "\n", \
      __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#  define log_arguments_debug(C, V) log_arguments_debug_helper( \
      (char*)__func__, C, V)
#endif

#define clean_errno() (errno == 0 ? "None" : strerror(errno))

#define log_err(M, ...) fprintf(stderr, "[ERROR] (%s:%d: errno: %s) " M "\n", \
    __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

#define log_warn(M, ...) fprintf(stderr, "[WARN] (%s:%d: errno: %s) " M "\n", \
    __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

#define log_info(M, ...) fprintf(stderr, "[INFO] (%s:%d) " M "\n", \
    __FILE__, __LINE__, ##__VA_ARGS__)

#define check(A, M, ...) if(!(A)) \
    { log_err(M, ##__VA_ARGS__); errno=0; goto error; }

#define sentinel(M, ...)  { log_err(M, ##__VA_ARGS__); errno=0; goto error; }

#define check_mem(A) check((A), "Out of memory.")

#define check_debug(A, M, ...) if(!(A)) \
    { debug(M, ##__VA_ARGS__); errno=0; goto error; }

void log_arguments_debug_helper(char *funcname, int argc, char *argv[]);

#endif
