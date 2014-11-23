#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "util.h"

char *concat_bytearray(char *s1, char *s2) {
    size_t len1 = strlen(s1);
    size_t len2 = strlen(s2);
    size_t outsz = len1+len2+1; // +1 for \0
    char *result = malloc(outsz); 
    if (!result) {
      fprintf(stderr, "Error allocating memory of size %zi\n", outsz);
    }

    memcpy(result, s1, len1);
    memcpy(result+len1, s2, len2+1); //+1 for \0
    return result;
  // TODO: how to free this memory? 
}

