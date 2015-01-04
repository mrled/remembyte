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

// In the config file, these characters are considered separators for elements
// that distinguish elements in a raw bytemap entry.
char *valid_map_separators = ", ";


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
  static int rmcount, bytectr; 
  static bool midprocessing;
  rawmap_type *current_rawmap;
  char *token;

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

    if (midprocessing) {
      rmcount = pconfig->rawmaps_count;
      current_rawmap = pconfig->rawmaps[rmcount-1];
    }
    else {
      midprocessing = true;

      bytectr = 0;
      rmcount +=1;
      pconfig->rawmaps_count = rmcount;
      pconfig->rawmaps = realloc(pconfig->rawmaps, 
        sizeof(rawmap_type*) * rmcount);

      current_rawmap = malloc(sizeof(rawmap_type));
      current_rawmap->name = strdup(name);
      pconfig->rawmaps[rmcount-1] = current_rawmap;
    }

    token = strtok( (char*)value2, valid_map_separators);
    //dbgprintf("bytectr: \n");
    for (; token != NULL; bytectr++) {
      //dbgprintf("  %i: '%s'\n", bytectr, token);
      current_rawmap->map[bytectr] = token;
      token = strtok(NULL, valid_map_separators);
    }

    if (bytectr == 256) { 
      midprocessing = false;
    }
    else if (bytectr > 256) {
      dbgprintf("inih_handler(): more than 256 values for raw map. "
        "Current bytectr == %i\n", bytectr);
      midprocessing = false;
      return 0;
    }

  }

  else if (safe_strcmp(section, "composedmap")) {

    dbgprintf("inih_handler(): Found a new composed map called '%s'\n", 
      name);

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
  char *inipath = "./remembyte.conf";
  int ix, iy;

  if (ini_parse(inipath, inih_handler, &config) < 0) {
    printf("Can't load 'test.ini'\n");
    return -1;
  }

  printf("Config loaded from %s: %i rawmaps, %i composedmaps\n", 
    inipath, config.rawmaps_count, config.composedmaps_count);

  printf("Rawmaps:\n");
  for (ix=0; ix<config.rawmaps_count; ix++) {
    printf("rawmap %i '%s':  ", ix, config.rawmaps[ix]->name);
    for (iy=0; iy<256; iy++) {
      printf("%s", config.rawmaps[ix]->map[iy]);
      if (iy <255) {
        printf(", ");
      }
      else {
        printf("\n");
      }
    }
    printf("\n\n");
  }
  return 0;
}
