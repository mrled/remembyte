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
  char *inipath = "./remembyte.conf";
  configuration_type *config;

  config = process_configfile(inipath);
  print_configuration_type(config, 0);
  return 0;
}
