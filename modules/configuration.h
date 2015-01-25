#ifndef _CONFIGURATION_H
#define _CONFIGURATION_H

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

#include "inih/ini.h"
#include "dbg.h"
#include "util.h"

#define BYTEMAX 256

/* A struct representing a raw map and its name
 */
typedef struct rawmap_struct {
  char *name;
  char *map[256];
} rawmap_type;

/* A struct representing the composed maps defined in the config file
 */
typedef struct composedmap_struct {
  char *name;
  int rawmaps_count;
  rawmap_type **rawmaps; // treated as an array of pointers to rawmap_type structs
  char ***rawmapsv; // treated as an array of &char[256] pointers
  char *separator;
  char *terminator;
} composedmap_type; 

/* A struct representing the configuration we get from the config file
 */
typedef struct configuration_struct {
  char *filepath;
  int rawmaps_count;
  rawmap_type **rawmaps; // treated as an array
  int composedmaps_count;
  composedmap_type **composedmaps; // treated as an array
  composedmap_type *defaultmap;
} configuration_type;

// These characters are considered separators for values in the config file
extern char *valid_value_separators;

configuration_type *configuration_new();
rawmap_type *rawmap_new();
composedmap_type *composedmap_new();

void rawmap_free(rawmap_type *rm);
void composedmap_free(composedmap_type *cm);
void configuration_free(configuration_type *cnf);
void configuration_free_recursive(configuration_type *cnf);

composedmap_type *a2composedmap_type(const char *, configuration_type *);
rawmap_type *a2rawmap_type(const char *, configuration_type *);

int inih_handler(void *, const char *, const char *, const char *);
configuration_type *process_configfile(const char *);
void print_configuration_type(configuration_type *, int);

#endif