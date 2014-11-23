/* Byte mappings */

#ifndef _BYTEMAPS_H
#define _BYTEMAPS_H

#define EMOJI_MAP_LEN 256
#define PGP_WORDLIST_TWO_LEN 256
#define PGP_WORDLIST_THREE_LEN 256

const char *emoji_map[EMOJI_MAP_LEN];
const char *pgp_wordlist_two[PGP_WORDLIST_TWO_LEN];
const char *pgp_wordlist_three[PGP_WORDLIST_THREE_LEN];

char *map_hexbuf_to_emoji(unsigned char *hash, size_t hash_len);

#endif
