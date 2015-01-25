#include "configuration.h"

char *valid_value_separators = ", "; // extern defined in bytemaps.h

// TODO: have a function to update the default composedmap in a configuration

/* Return a pointer to a new configuration_type struct
 */
configuration_type *configuration_new() {
  configuration_type *config=NULL;
  rawmap_type *hexrmap=NULL;
  composedmap_type *hexcmap=NULL;
  char *hexstring=NULL;
  int hexbyte_sz=3;
  int ix;

  config = malloc(sizeof(configuration_type));
  check_mem(config);
  config->rawmaps = NULL;
  config->composedmaps = NULL;
  config->filepath = NULL;
  config->defaultmap = NULL;

  // Make the default hex rawmap and composedmap
  hexrmap = rawmap_new();
  check_mem(hexrmap);
  hexrmap->name = "hex";
  for (ix=0; ix<BYTEMAX; ix++) {
    hexrmap->map[ix] = NULL;
  }
  for (ix=0; ix<BYTEMAX; ix++) {
    hexstring = malloc(sizeof(char) *hexbyte_sz);
    check_mem(hexstring);
    snprintf(hexstring, hexbyte_sz, "%02x", ix);
    hexrmap->map[ix] = hexstring;
  }

  config->rawmaps_count = 1;
  config->rawmaps = malloc(
    sizeof(rawmap_type*) * config->rawmaps_count);
  check_mem(config->rawmaps);
  config->rawmaps[0] = hexrmap;

  hexcmap = composedmap_new();
  check_mem(hexcmap); 
  hexcmap->name = "hex";
  hexcmap->rawmaps_count = 1;
  hexcmap->rawmaps = malloc(sizeof(rawmap_type*) * hexcmap->rawmaps_count);
  check_mem(hexcmap->rawmaps);
  hexcmap->rawmaps[0] = hexrmap;
  hexcmap->rawmapsv = malloc(sizeof(char**) * hexcmap->rawmaps_count);
  check_mem(hexcmap->rawmapsv);
  hexcmap->rawmapsv[0] = hexrmap->map;
  hexcmap->separator = ":";
  hexcmap->terminator = "";

  config->composedmaps_count = 1;
  config->composedmaps = malloc(
    sizeof(composedmap_type*) * config->composedmaps_count);
  check_mem(config->composedmaps);
  config->composedmaps[0] = hexcmap;
  config->defaultmap = hexcmap;

  return config;

error:
  if (config) {
    free(config->rawmaps);
    free(config->composedmaps);
    free(config);
  }
  if (hexcmap) {
    free(hexcmap->rawmaps);
    free(hexcmap->rawmapsv);
    free(hexcmap);
  }
  if (hexrmap) {
    for (ix=0; ix<BYTEMAX; ix++) {
      free(hexrmap->map[ix]);
    }
    free(hexrmap);
  }
  return NULL;
}

composedmap_type *composedmap_new() {
  composedmap_type *cmap;
  cmap = malloc(sizeof(composedmap_type));
  check_mem(cmap);
  cmap->name = "";
  cmap->rawmaps_count = 0;
  cmap->rawmaps = NULL;
  cmap->rawmapsv = NULL;
  cmap->separator = "";
  cmap->terminator = "";
  return cmap;
error:  
  free(cmap);
  return NULL;
}

rawmap_type *rawmap_new() {
  int ix;
  rawmap_type *rmap;
  rmap = malloc(sizeof(rawmap_type));
  check_mem(rmap);
  rmap->name = "";
  for (ix=0; ix<BYTEMAX; ix++) {
    rmap->map[ix] = NULL;
  }
  return rmap;
error:
  return NULL;
}

/* Free a rawmap_type struct
 */
void rawmap_free(rawmap_type *rm) {
  free(rm->map);
  free(rm);
}

/* Free a composedmap_type struct. 
 * Note that the rawmap_type structs themselves are *not* freed, only the array
 * of pointers to them. 
 */
void composedmap_free(composedmap_type *cm) {
  free(cm->rawmaps);
  free(cm->rawmapsv);
  free(cm);
}

/* Free a configuration_type struct
 * Note that the rawmap and composedmap structs are *not* freed, only the 
 * arrays of pointers to them.
 */
void configuration_free(configuration_type *cnf) {
  free(cnf->rawmaps);
  free(cnf->composedmaps);
  free(cnf);
}

/* Free a configuration_type struct, and any composedmap or rawmap structs that
 * it pointers to.
 */
void configuration_free_recursive(configuration_type *cnf) {
  int ix; 
  for (ix=0; ix< cnf->composedmaps_count; ix++) {
    composedmap_free( cnf->composedmaps[ix] );
  }
  for (ix=0; ix< cnf->rawmaps_count; ix++) {
    rawmap_free( cnf->rawmaps[ix] );
  }
  configuration_free(cnf);
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
        pconfig->defaultmap = current_composedmap;
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
    if (current_composedmap == config->defaultmap) {
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
