/* Byte mappings */

#ifndef _BYTEMAPS_H
#define _BYTEMAPS_H

const int emoji_map_len;
const char *emoji_map[];
const int pgp_wordlist_two_len;
const char *pgp_wordlist_two[];
const int pgp_wordlist_three_len;
const char *pgp_wordlist_three[];

char *map_hexbuf_to_emoji(unsigned char *hash, size_t hash_len);

#endif
