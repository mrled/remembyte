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

  config.filepath = inipath;

  if (ini_parse(inipath, inih_handler, &config) < 0) {
    printf("Can't load 'test.ini'\n");
    return -1;
  }

  print_configuration_type(&config, 0);
  return 0;
}
