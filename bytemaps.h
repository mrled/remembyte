#ifndef _BYTEMAPS_H
#define _BYTEMAPS_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <libssh/libssh.h>
#include <pcre.h>

#include "remembyte.h"

// bytemaps are arrays w/ 256 null-terminated strings - one per possible byte value
#define MAPPING_SIZE 256

const char *emoji_map[MAPPING_SIZE];
const char *pgp_wordlist_two[MAPPING_SIZE];
const char *pgp_wordlist_three[MAPPING_SIZE];

typedef enum {
  HEX,
  EMOJI,
  PGP
} mapping_t;
mapping_t a2mapping_t(char *);

int hex2buf(char *, unsigned char **, int *);

char *get_display_hash(unsigned char *, size_t, mapping_t);
char *buf2map(unsigned char *, size_t , const char *, const char *, char ***, size_t);

#endif
