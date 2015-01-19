#include "dbg.h"

void log_arguments_debug_helper(char *funcname, int argc, char *argv[]) {
    int ix;
    char *arg_string;
    fprintf(stderr, "[ARGS DEBUG] %s(", funcname);
    for (ix=0; ix<argc; ix++) {
        if (ix < argc -1) {
            arg_string = "%s, ";
        }
        else {
            arg_string = "%s";
        }
        fprintf(stderr, arg_string, argv[ix]);
    }
    fprintf(stderr, ")\n");
}