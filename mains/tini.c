/* Example: parse a simple configuration file */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../modules/inih/ini.h"

#include "../modules/util.h"
#include "../modules/bytemaps.h"

int main(int argc, char* argv[])
{
  char *inipath = "./remembyte.conf";
  configuration_type *config;

  config = process_configfile(inipath);
  print_configuration_type(config, 1);


  configuration_type *config2;
  int inip_ret;

  config2 = configuration_new();
  config2->filepath = strdup(inipath);
  inip_ret = ini_parse(inipath, inih_handler, config2);
  printf("ini_parse() return value: %i\n", inip_ret);
  print_configuration_type(config2, 1);


  return 0;
}
