#include "bytemaps.h"

/* Transform a hex string to a byte array that the hex string represents
 * 
 * @param hexstring a string of the form '0x12ab...' or '12:ab:...' or 
 *        '12ab...'
 *
 * @param outbuffer a pointer which will be set to the byte array
 * 
 * @return the length of the outbuffer if successful, a negative number otherwise
 */
size_t hex2buf(char * hexstring, unsigned char ** outbuffer) {
  char *normhs=NULL;
  unsigned char *obuf=NULL;
  size_t outbuffer_len=0;

  normhs = normalize_hexstring(hexstring);
  check(normhs, "The string %s is not a hex string\n", hexstring);

  *outbuffer = nhex2int(normhs);
  check(*outbuffer, "The string %s is not a hex string\n", hexstring);

  outbuffer_len = strlen(normhs)/2;

  free(normhs);
  return outbuffer_len;

error:
  free(normhs);
  return -1;
}

/* Convert a hit (a _H_ex dig_IT_) to an integer 
 * (used in nhex2int())
 *
 * Cribbed from: http://stackoverflow.com/questions/12535320/
 * See also: http://stackoverflow.com/questions/5089701/
 * 
 * @param hit a hex digit, in the class [0-9a-f]. Uppercase letters are not
 *        supported.
 * 
 * @return an integer value between 0-15.
 *
 * TODO: make this work with upper case hits as well
 * TODO: what happens if you pass bad data here? 
 */
static inline int hit2int(char hit) {
  check((('0'<=hit<='9') && ('a'<=hit<='f')), "Bad argument: '%c'", hit);
  return ((hit) <= '9' ? (hit) - '0' : (hit) - 'a' + 10);
error:
  return -1;
}

/* Normalize a string containing a hexadecimal representation of a number
 * 
 * @param hexstring a string containing a hex representation of a number
 *
 * @return a normalized, guaranteed lower-case, null-terminated hexstring 
 *         without leading '0x' or inter-byte ':' characters. if the hexstring
 *         is invalid, return NULL. the returned pointer must be freed by the
 *         caller.
 */
char * normalize_hexstring(char * hexstring) {
  unsigned int iidx=0, oidx=0;
  char ca, cb;
  char *hexstring_norm=NULL;

  hexstring_norm = malloc(sizeof(char) * strlen(hexstring) +1);
  check_mem(hexstring_norm);

  if (hexstring[0] == '0' && hexstring[1] == 'x') {
    iidx = 2;
  }

  while (iidx < strlen(hexstring)) {

    ca = hexstring[iidx];
    cb = hexstring[iidx +1];

    if ('A'<=ca<='F') ca = tolower(ca);
    if ('A'<=cb<='F') cb = tolower(cb);

    check((hit2int(ca) > -1 && hit2int(cb) > -1), "Bad input");
#ifdef REMEMBYTE_DEBUG_NORMHS
    log_debug("ca = %c, cb = %c", ca, cb);
#endif

    if (ca != ':') {
      hexstring_norm[oidx] = ca;
      hexstring_norm[oidx+1] = cb;
      oidx += 2;
      iidx += 2;
    }
    else {
      iidx++;
    }
  }
  log_debug("Original hexstring: '%s'; normalized: '%s'", hexstring, 
    (char*)hexstring_norm);

  return hexstring_norm;

error:
  free(hexstring_norm);
  return NULL;
}

/* Convert a normalized hex string to a byte array that the hex string 
 * represents
 * 
 * @param hexstring a string with any number of pairs of hex digits, without
 *        leading '0x' or inter-byte ':' characters
 * 
 * @return a buffer strlen(hexstring)/2 bytes long containing the byte array
 *         represented by hexstring
 */
unsigned char * nhex2int(char * hexstring) {
  size_t buffer_len;
  unsigned char *buffer=NULL;
  long ix;
  int ia, ib;

  buffer_len = strlen(hexstring) / 2;
  buffer = malloc(buffer_len);
  check_mem(buffer);

  for (ix=0; ix<buffer_len; ix++) {
    ia = hit2int(hexstring[2 * ix + 0]);
    ib = hit2int(hexstring[2 * ix + 1]);
    check ((ia >=0 && ib >=0), "Bad input");
    // shift ia four bits to the left, because four bits is half of one byte 
    // and ia is the first half of the byte representation.
    buffer[ix] = (ia << 4) | ib;
  }
  return buffer;

error:
  free(buffer);
  return NULL;
}

char *get_display_hash(
  unsigned char *hash, 
  size_t hash_len, 
  composedmap_type *cmap)
{
  char *mapped_buffer=NULL;

  check(cmap, "Bad map");
  check(hash_len >0, "Passed empty buffer");

  mapped_buffer = buf2map(hash, hash_len, 
    cmap->separator, cmap->terminator, cmap->rawmapsv, cmap->rawmaps_count);
  check(mapped_buffer, "Error mapping buffer");

  return mapped_buffer;
  // NOTE: Caller must free the returned pointer

error:
  free(mapped_buffer);
  return NULL;
}

/**
 * Map the bytes in a buffer to values from one or more bytemaps
 * 
 * @param buffer a buffer
 *
 * @param buflen the length of the buffer in bytes
 *
 * @param separator null-terminated string to insert between each byte representation
 *
 * @param terminator null-terminated string to insert after the last byte representation 
 *
 * @param maps an array of pointers to bytemap arrays. bytemap arrays are arrays with 256 elements, where each element is a null-terminated string.
 *
 * @param maps_count the number of maps passed
 *
 * @return a null-terminated string representing the 'buffer' parameter. each byte in the original buffer is represented by one of the bytemaps passed as the 'maps' parameter; between each pair of byte representations is the separator string, and after the representation of the last byte is the terminator string. maps are used alternatingly, in the order provided - for example, if 2 maps are passed, the first byte will be represented by its value from maps[0], the second from maps[1], the third from maps[0] again, and so on. 
 *
 */
char *
buf2map(
  unsigned char *buffer, 
  size_t buflen, 
  const char *separator, 
  const char *terminator, 
  char *** maps,
  size_t maps_count)
{
  char *byterep=NULL, *os=NULL, *new_os=NULL;
  int inctr=0, ossz=0, insertpt=0, mapnum;
  size_t septerm_longest=0, separator_sz=0, terminator_sz=0, byterep_sz=0;
  unsigned int singlebyte;

#ifdef REMEMBYTE_DEBUG_BUF2MAP
  log_debug("Arguments: "
    "\n  unsigned char buffer = (undisplayable)," 
    "\n  size_t buflen = '%zi',"
    "\n  const char *separator = '%s',"
    "\n  const char *terminator = '%s',"
    "\n  char ***maps = (out parameter),"
    "\n  size_t maps_count = (out parameter))", 
    buflen, separator, terminator);
#endif 

  check(buflen >0, "Tried to map a buffer with a length of 0");
  check(maps_count >= 0, "Tried to map a buffer without passing any maps");

  if (separator)  { separator_sz  = strlen(separator);  }
  if (terminator) { terminator_sz = strlen(terminator); }

  if (separator_sz > terminator_sz) {
    septerm_longest = separator_sz;
  }
  else {
    septerm_longest = terminator_sz +1; 
  }

  for (inctr=0; inctr<buflen; inctr++) {
    singlebyte = (unsigned int)buffer[inctr];

    mapnum = inctr % maps_count;
    byterep = (char*) maps[mapnum][singlebyte];
    byterep_sz = strlen(byterep);

    insertpt = ossz;
    ossz += byterep_sz + septerm_longest;

    new_os = realloc(os, ossz); 
    check_mem(new_os);
    os = new_os;

    memcpy(os +insertpt, byterep, byterep_sz);
    insertpt += byterep_sz;

    if (inctr != buflen-1) {
      memcpy(os +insertpt, separator, separator_sz);
    }
    else {
      memcpy(os +insertpt, terminator, terminator_sz +1); // +1 for terminating \0
    }

  }
  return os;
  // NOTE: Caller must free the returned pointer

error:
  free(os);
  free(new_os);
  return NULL;
}

