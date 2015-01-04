/* Example: parse a simple configuration file */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "inih/ini.h"
#include "util.h"

bool DEBUGMODE = true; // extern defined in util.h

/* A struct representing a raw map and its name
 */
typedef struct {
  char *name;
  char *map[256];
} rawmap_type;

/* A struct representing the composed maps defined in the config file
 */
typedef struct {
  char *name;
  int rawmaps_count;
  rawmap_type **rawmaps; // treated as an array
  char *separator;
  char *terminator;
} composedmap_type; 

/* A struct representing the configuration we get from the config file
 */
typedef struct {
  int rawmaps_count;
  rawmap_type **rawmaps; // treated as an array
  int composedmaps_count;
  composedmap_type **composedmaps; // treated as an array

} configuration_type;

// These characters are considered separators for values in the config file
char *valid_value_separators = ", \0";

composedmap_type * a2composedmap_type(
  const char *name,
  configuration_type config) 
{
  int ix;
  composedmap_type *ret;
  ret = NULL;
  for (ix=0; ix<config.composedmaps_count; ix++) {
    if (safe_strcmp(config.composedmaps[ix]->name, name)) {
      ret = config.composedmaps[ix];
      break;
    }
  }
  return ret;
}

rawmap_type * a2rawmap_type(
  const char *name,
  configuration_type config) 
{
  int ix;
  rawmap_type *ret;
  ret = NULL;
  for (ix=0; ix<config.rawmaps_count; ix++) {
    if (safe_strcmp(config.rawmaps[ix]->name, name)) {
      ret = config.rawmaps[ix];
      break;
    }
  }
  return ret;
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
static int inih_handler(
  void *configuration, 
  const char *section, 
  const char *name,
  const char *value)
{

  configuration_type *pconfig = (configuration_type*) configuration;

  // static local variables are initialized to 0
  static int rmcount, cmcount, bytectr; 
  rawmap_type *current_rawmap;
  composedmap_type *current_composedmap;
  char *token, *cmname;

  int ix;

  // value2 exists b/c strtok() modifies the string passed to it; we need to 
  // copy 'value' into 'value2' so that the INI parsing library doesn't barf
  char *value2; 
  value2 = malloc(strlen(value) +1);  
  memcpy(value2, value, strlen(value) +1);


  /* This will print out each line of the config file as its being parsed:
  static int linectr;
  linectr +=1;
  dbgprintf("inih_handler(): "
    "%s (%zi) // %s (%zi) // %s (%zi). byte: %i // line: %i \n", 
    section, strlen(section), name, strlen(name), value, strlen(value), 
    bytectr, linectr);
  */

  if (strlen(section) == 0) {
    dbgprintf("inih_handler(): in general section\n");
  }

  if (safe_strcmp(section, "rawmaps")) {

    current_rawmap = a2rawmap_type(name, *pconfig);
    if (!current_rawmap) {
      bytectr = 0;
      rmcount +=1;
      pconfig->rawmaps_count = rmcount;
      pconfig->rawmaps = realloc(pconfig->rawmaps, 
        sizeof(rawmap_type*) * rmcount);

      current_rawmap = malloc(sizeof(rawmap_type));
      current_rawmap->name = strdup(name);
      pconfig->rawmaps[rmcount-1] = current_rawmap;
    }

    token = strtok( (char*)value2, valid_value_separators);
    //dbgprintf("bytectr: \n");
    for (; token != NULL; bytectr++) {
      //dbgprintf("  %i: '%s'\n", bytectr, token);
      current_rawmap->map[bytectr] = token;
      token = strtok(NULL, valid_value_separators);
    }

    if (bytectr == 256) { 
    }
    else if (bytectr > 256) {
      dbgprintf("inih_handler(): more than 256 values for raw map. "
        "Current bytectr == %i\n", bytectr);
      return 0;
    }

  }

  else if (safe_strncmp(section, "composedmap", strlen("composedmap"))) {

    cmname = (char*) &(section[ strlen("composedmap ") ]);

    current_composedmap = a2composedmap_type(cmname, *pconfig);
    if (!current_composedmap) {

      cmcount +=1;
      pconfig->composedmaps_count = cmcount;
      pconfig->composedmaps = realloc(pconfig->composedmaps,
        sizeof(composedmap_type*) * cmcount);

      current_composedmap = malloc(sizeof(composedmap_type));
      current_composedmap->name = strdup(cmname);
      pconfig->composedmaps[cmcount-1] = current_composedmap;
    }

    if (safe_strcmp(name, "rawmaps")) {

      token = strtok( (char*)value2, valid_value_separators);
      for (ix=0; token != NULL; ix++) {

        current_rawmap = a2rawmap_type(token, *pconfig);
        current_composedmap->rawmaps_count +=1;
        current_composedmap->rawmaps = realloc(current_composedmap->rawmaps,
          sizeof(rawmap_type*) * current_composedmap->rawmaps_count);
        current_composedmap->rawmaps[ix] = current_rawmap;

        token = strtok(NULL, valid_value_separators);
      }
    }

    else if (safe_strcmp(name, "separator") || 
      safe_strcmp(name, "terminator")) 
    {
      if (value2[0] == '"') {
        value2 = &value2[1];
      }
      if (value2[ strlen(value2) -1 ] == '"') {
        value2[ strlen(value2) -1 ] = '\0';
      }

      if (safe_strcmp(name, "separator")) {
        current_composedmap->separator = strdup(value2);
      }
      else if (safe_strcmp(name, "terminator")) {
        current_composedmap->terminator = strdup(value2);
      }
    }

    else {
      dbgprintf("Ignoring extra field in config file: '%s'\n", name);
    }

  }

  else {
    dbgprintf("inih_handler(): Unknown section: '%s'\n", section);
    return 0;
  }

  return 1;
}

int main(int argc, char* argv[])
{
  configuration_type config;
  rawmap_type *current_rawmap;
  composedmap_type *current_composedmap;
  char *inipath = "./remembyte.conf";
  int ix, iy;

  if (ini_parse(inipath, inih_handler, &config) < 0) {
    printf("Can't load 'test.ini'\n");
    return -1;
  }

  printf("Config loaded from %s: %i rawmaps, %i composedmaps\n", 
    inipath, config.rawmaps_count, config.composedmaps_count);

  for (ix=0; ix<config.rawmaps_count; ix++) {
    current_rawmap = config.rawmaps[ix];
    printf("- rawmap %i '%s'", ix, current_rawmap->name);
    /*
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
    */
    printf("\n");
  }

  for (ix=0; ix<config.composedmaps_count; ix++) {
    current_composedmap = config.composedmaps[ix];
    printf("- composedmap %i '%s'\n", 
      ix, current_composedmap->name);
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

  return 0;
}
