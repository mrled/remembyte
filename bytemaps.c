#include "bytemaps.h"

char *valid_value_separators = ", \0"; // extern defined in bytemaps.h

// TODO: I need *_free() functions to match all these *_new() functions

/* Return a pointer to a new configuration_type struct
 */
configuration_type *configuration_new() {
  configuration_type *config;
  config = malloc(sizeof(configuration_type));
  check_mem(config);
  config->filepath = NULL;
  config->rawmaps_count = 0;
  config->rawmaps = NULL;
  config->composedmaps_count = 0;
  config->composedmaps = NULL;
  return config;
error:
  return NULL;
}

composedmap_type *composedmap_new() {
  composedmap_type *cmap;
  cmap = malloc(sizeof(composedmap_type));
  check_mem(cmap);
  cmap->name = "";
  cmap->isdefault = false;
  cmap->rawmaps_count = 0;
  cmap->rawmaps = NULL;
  cmap->rawmapsv = NULL;
  cmap->separator = "";
  cmap->terminator = "";
  return cmap;
error:  
  return NULL;
}

rawmap_type *rawmap_new() {
  rawmap_type *rmap;
  rmap = malloc(sizeof(rawmap_type));
  check_mem(rmap);
  rmap->name = "";
  return rmap;
error:
  return NULL;
}

/* Take in a string representing the path to a config file, parse that file, 
 * and return a pointer to a configuration_type struct representing that file.
 */
configuration_type *process_configfile(const char * filename) {
  configuration_type *config=NULL;
  config = configuration_new();
  check_mem(config);
  config->filepath = strdup(filename);
  if (ini_parse(filename, inih_handler, config) < 0) {
    sentinel("Can't load config file '%s'\n", filename);
  }
  return config;
error:
  free(config);
  return NULL;
}

/* Take in the name of a composedmap and a configuration_type, and return a
 * pointer to the composedmap_type referred to by the name.
 */
composedmap_type * a2composedmap_type(
  const char *name,
  configuration_type *config) 
{
  int ix;
  for (ix=0; ix<config->composedmaps_count; ix++) {
    if (safe_strcmp(config->composedmaps[ix]->name, name)) {
      return config->composedmaps[ix];
    }
  }
  return NULL;
}

/* Take in a configuration_type pointer and return a pointer to the 
 * default composedmap_type if present, or NULL otherwise
 */
composedmap_type * get_default_map(
  configuration_type *config) 
{
  int ix;
  for (ix=0; ix<config->composedmaps_count; ix++) {
    if (config->composedmaps[ix]->isdefault) {
      return config->composedmaps[ix];
    }
  }
  return NULL;
}

/* Take in the name of a rawmap and a configuration_type, and return a
 * pointer to the rawmap_type referred to by the name.
 */
rawmap_type * a2rawmap_type(
  const char *name,
  configuration_type *config) 
{
  int ix;
  for (ix=0; ix<config->rawmaps_count; ix++) {
    if (safe_strcmp(config->rawmaps[ix]->name, name)) {
      return config->rawmaps[ix];
    }
  }
  return NULL;
}

/* Callback function for inih library. 
 * 
 * Called once per line of the config file.
 * 
 * nonreentrant: uses strtok(), static local vars
 *
 * TODO: 
 * - error checking?  what's that? 
 * 
 * @param configuration a pointer to a struct that will contain our config data
 * @param section the name of the current section 
 * @param name the name of the configuration item (the part to the left of the
 *        equals sign)
 * @param value the value of the configuration item (the part to the right of
 *        the equals sign)
 */
int inih_handler(
  void *configuration, 
  const char *section, 
  const char *name,
  const char *value)
{

  configuration_type *pconfig = (configuration_type*) configuration;

  static int bytectr=0; 

  rawmap_type *current_rawmap;
  composedmap_type *current_composedmap;
  char *token;
  int ix;

#ifdef REMEMBYTE_DEBUG_CONFIG_PARSER
  // This will print out each line of the config file as its being parsed:
  static int linectr=0;
  log_debug("%s (%zi) // %s (%zi) // %s (%zi). byte: %i // line: %i \n", 
    section, strlen(section), name, strlen(name), value, strlen(value), 
    bytectr, linectr++);
#endif

  if (strlen(section) == 0) {
    log_debug("In general section, but nothing to do here");
  }

  if (safe_strcmp(section, "rawmaps")) {
    char *value2 = strdup(value);
    current_rawmap = a2rawmap_type(name, pconfig);
    if (!current_rawmap) {
      bytectr = 0;
      pconfig->rawmaps_count += 1; 
      pconfig->rawmaps = realloc(pconfig->rawmaps, 
        sizeof(rawmap_type*) * pconfig->rawmaps_count);

      current_rawmap = malloc(sizeof(rawmap_type));
      current_rawmap->name = strdup(name);
      pconfig->rawmaps[ pconfig->rawmaps_count -1 ] = current_rawmap;
    }

    token = strtok( (char*)value2, valid_value_separators);
    for (; token != NULL; bytectr++) {
      current_rawmap->map[bytectr] = strdup(token);
      token = strtok(NULL, valid_value_separators);
    }
    
    free(value2);

    check(0 <= bytectr <= 256, "Bad value for bytectr: %i", bytectr) ;
  }

  else if (safe_strncmp(section, "composedmap", strlen("composedmap"))) {

    char *cmname = (char*) &(section[ strlen("composedmap ") ]);

    current_composedmap = a2composedmap_type(cmname, pconfig);
    if (!current_composedmap) {

      pconfig->composedmaps_count += 1;
      pconfig->composedmaps = realloc(pconfig->composedmaps,
        sizeof(composedmap_type*) * pconfig->composedmaps_count);

      current_composedmap = malloc(sizeof(composedmap_type));
      current_composedmap->name = strdup(cmname);
      current_composedmap->rawmaps_count = 0;
      current_composedmap->rawmaps = NULL;
      current_composedmap->rawmapsv = NULL;
      pconfig->composedmaps[ pconfig->composedmaps_count -1 ] = 
        current_composedmap;
    }

    if (safe_strcmp(name, "rawmaps")) {
      
      char *value2 = strdup(value);

      // TODO: I don't think my code handles multiple lines in this section

      token = strtok( (char*)value2, valid_value_separators);
      for (ix=0; token != NULL; ix++) {

        current_rawmap = a2rawmap_type(token, pconfig);
        current_composedmap->rawmaps_count +=1;
        current_composedmap->rawmaps = realloc(current_composedmap->rawmaps,
          sizeof(rawmap_type*) * current_composedmap->rawmaps_count);
        current_composedmap->rawmaps[ix] = current_rawmap;

        current_composedmap->rawmapsv = realloc(current_composedmap->rawmapsv,
          sizeof(char **) * current_composedmap->rawmaps_count);
        current_composedmap->rawmapsv[ix] = current_rawmap->map;

        token = strtok(NULL, valid_value_separators);
      }
      
      free(value2);
    }

    else if (safe_strcmp(name, "separator") || 
      safe_strcmp(name, "terminator")) 
    {
      char *value2 = strdup(value);
      char *v2idx = value2;

      // Remove initial double quote marks
      if (v2idx[0] == '"') {
        v2idx = &value2[1];
      }
      // Remove terminal double quote marks
      if (v2idx[ strlen(v2idx) -1 ] == '"') {
        v2idx[ strlen(v2idx) -1 ] = '\0';
      }

      if (safe_strcmp(name, "separator")) {
        current_composedmap->separator = strdup(v2idx);
      }
      else if (safe_strcmp(name, "terminator")) {
        current_composedmap->terminator = strdup(v2idx);
      }
      
      free(value2);
    }

    else if (safe_strcmp(name, "default")) {
      if (str2bool((char*)value)) {
        current_composedmap->isdefault = true;
      }
    }

    else {
      log_debug("Ignoring extra field in config file: '%s'\n", name);
    }

  }

  else {
    sentinel("Unknown section '%s'", section);
  }

  // TODO: Audit this function for what needs to be free()'d 

  return 1;

error: 
  return 0;
}

/* Print a configuration_type struct
 * @param config a configuration_type struct
 * @param verbosity the verbosity level to use. 
 *        - if '0', print rawmaps and composedmaps only
 *        - if '1', also print each element in each rawmap
 */
void print_configuration_type(
  configuration_type *config,
  int verbosity)
{
  rawmap_type *current_rawmap;
  composedmap_type *current_composedmap;
  int ix, iy;
  char *defaultness_text, *fp_text;

  if (verbosity < 0) {
    log_debug("Verbosity incorrectly set to %i; changing to 0", verbosity);
    verbosity = 0;
  }
  else if (verbosity > 1) {
    log_debug("Verbosity incorrectly set to %i; changing to 1", verbosity);
    verbosity = 1;
  }

  if (config->filepath) {
    fp_text = config->filepath;
  }
  else {
    fp_text = "unknown file";
  }
  printf("Config (loaded from %s): %i rawmaps, %i composedmaps\n", 
    fp_text, config->rawmaps_count, config->composedmaps_count);

  for (ix=0; ix<config->rawmaps_count; ix++) {
    current_rawmap = config->rawmaps[ix];
    printf("- rawmap %i '%s'", ix, current_rawmap->name);

    if (verbosity >= 1) {
      // print all of the elements of each rawmap
      printf(": ");
      for (iy=0; iy<256; iy++) {
        printf("%s", current_rawmap->map[iy]);
        if (iy <255) {
          printf(", ");
        }
        else {
          printf("\n");
        }
      }
    }

    printf("\n");
  }

  for (ix=0; ix<config->composedmaps_count; ix++) {
    current_composedmap = config->composedmaps[ix];
    if (current_composedmap->isdefault) {
      defaultness_text = "DEFAULT";
    }
    else {
      defaultness_text = "not default";
    }
    printf("- composedmap %i '%s' (%s)\n", 
      ix, current_composedmap->name, defaultness_text);
    printf("  - Uses %i rawmap(s): ", current_composedmap->rawmaps_count);
    for (iy=0; iy<current_composedmap->rawmaps_count; iy++) {
      current_rawmap = current_composedmap->rawmaps[iy];
      printf("%s", current_rawmap->name);
      if (iy != current_composedmap->rawmaps_count -1) {
        printf(", ");
      }
    }
    printf("\n");
    printf("  - Separator: '%s'\n  - Terminator: '%s'\n", 
      current_composedmap->separator, current_composedmap->terminator);
  }

}


// TODO: deal with Unicode. These functions will work with ASCII and UTF-8, but
// nothing else


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
 * TODO: why does use of the `inline` keyword produce no errors on Mac OS X but 
 *       fail to compile on Linux? 
 * TODO: make this work with upper case hits as well
 * TODO: what happens if you pass bad data here? 
 */
//inline int hit2int(char hit) {
int hit2int(char hit) {
  check((('0'<=hit<='9') && ('a'<=hit<='f')), "Bad argument: '%c'", hit);
  /*
  if (! (('0'<=hit<='9') || ('a'<=hit<='f'))) {
    fprintf(stderr, "Bad argument to hit2int(): '%c'\n", hit);
    return -1;
  }
  */
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
    log_debug("ca = %c, cb = %c", ca, cb);

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

  check(maps_count >= 0, "Tried to map a buffer without passing any maps");

  if (separator)  { separator_sz  = strlen(separator);  }
  if (terminator) { terminator_sz = strlen(terminator); }

  if (separator_sz > terminator_sz) {
    septerm_longest = separator_sz;
  }
  else {
    // +1 for the \0 that will terminate the mapped string
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

