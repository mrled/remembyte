/* Example: parse a simple configuration file */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "inih/ini.h"
#include "util.h"
#include "bytemaps.h"

bool DEBUGMODE = true; // extern defined in util.h


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
