#ifndef _BYTEMAPS_H
#define _BYTEMAPS_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <libssh/libssh.h>

#include "util.h"

// Is there a better way to do these? 
#define EMOJI_MAP_LEN 256
#define PGP_WORDLIST_TWO_LEN 256
#define PGP_WORDLIST_THREE_LEN 256

const char *emoji_map[EMOJI_MAP_LEN];
const char *pgp_wordlist_two[PGP_WORDLIST_TWO_LEN];
const char *pgp_wordlist_three[PGP_WORDLIST_THREE_LEN];

typedef enum {
  HEX,
  EMOJI
} mapping_t;
mapping_t a2mapping_t(char *);

void get_display_hash(unsigned char *, size_t, mapping_t, char **);

char *buf2emoji(unsigned char *, size_t);
char *buf2hex(unsigned char *, size_t);

#endif
