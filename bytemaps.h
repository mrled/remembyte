#ifndef _BYTEMAPS_H
#define _BYTEMAPS_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

#include <libssh/libssh.h>

#include "util.h"

// bytemaps are arrays w/ 256 null-terminated strings - one per possible byte value
#define MAPPING_SIZE 256

const char *emoji_map[MAPPING_SIZE];
const char *pgp_wordlist_two[MAPPING_SIZE];
const char *pgp_wordlist_three[MAPPING_SIZE];

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
  bool isdefault;
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

} configuration_type;

// These characters are considered separators for values in the config file
//char *valid_value_separators = ", \0";
extern char *valid_value_separators;

composedmap_type * a2composedmap_type(const char *, configuration_type *);
composedmap_type * get_default_map(configuration_type *);
rawmap_type * a2rawmap_type(const char *, configuration_type *);
int inih_handler(void *, const char *, const char *, const char *);

typedef enum {
  NOMAPPING,
  HEX,
  EMOJI,
  PGP
} mapping_t;
mapping_t a2mapping_t(char *);

int hex2buf(char *, unsigned char **);
bool validate_hexstring(char *);
char * normalize_hexstring(char *);
unsigned char * nhex2int(char *);

char *get_display_hash(unsigned char *, size_t, mapping_t);
char *buf2map(unsigned char *, size_t , const char *, const char *, char ***, size_t);

#endif
