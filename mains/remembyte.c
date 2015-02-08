#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <libssh/libssh.h>

#include "../modules/configuration.h"
#include "../modules/inih/ini.h"
#include "../modules/bytemaps.h"
#include "../modules/act_ssh.h"
#include "../modules/util.h"
#include "../modules/dbg.h"

#ifndef INSTALL_PREFIX
#  define INSTALL_PREFIX "/usr/local"
#endif

char *ARGV0;

typedef struct action_struct {
  int argc;
  char **argv;
  composedmap_type *cmap;
  int (*func) (configuration_type*, composedmap_type*, int, char*[]);
} action_type;

action_type *action_new() {
  action_type *action = malloc(sizeof(action_type));
  check_mem(action);
  action->argc = 0;
  action->argv = NULL;
  action->cmap = NULL;
  //check_mem(action->cmap);
  action->func = NULL;
  return action;
error:
  free(action);
  return NULL;
}

/* Free an action_type
 * Note that the composedmap it references is not freed, because it may still 
 * be used in the configuration
 */ 
void action_free(action_type *action) {
  free(action->argv);
  free(action);
  action = NULL;
}

void remembyte_help() {
  printf("%s: experiments in SSH key fingerprint display\n", ARGV0);
  printf("%s [-mFDh] [SUBCOMMAND] [SUBCOMMAND OPTIONS]\n", ARGV0);
  printf("    -m MAPNAME. Maps are defined in the config file\n");
  printf("    -F CONFIGFILE. Default is ~/.remembyte.conf\n");
  printf("    -h: display this message\n");
  printf("    SUBCOMMAND: one of the following:\n");
  printf("     -  ssh: connects to an SSH server and prints a key fingerprint\n");
  printf("        arguments: [HOSTNAME] [PORT]\n");
  printf("     -  map: prints the selected map from 0x00-0xff\n");
  printf("        arguments: none\n");
  printf("     -  input: takes input on the command line\n");
  printf("        arguments: INPUTSTRING [INPUTSTRING [...]] (as hex)\n");
  printf("     -  stdin (default): takes input from STDIN (as hex)\n");
  printf("        arguments: none\n");
}

int do_ssh_action(
  configuration_type *config,
  composedmap_type *cmap,
  int argc, 
  char *argv[]) 
{
  int actr, bctr;
  char *hostname, *port, *dhash, *argument, *hktype_name;
  ssh_hostkeys *hostkeys=NULL;
  ssh_banners *banners=NULL;
  ssh_session session=NULL;

  log_arguments_debug(argc, argv);

  // Set defaults...
  hostname = "localhost";
  port = "22";

  for (actr=0, bctr=0; actr<argc; actr++) {
    argument = argv[actr];
    check(argument[0] != '-', "Bad argument: '%s'", argument);
    switch (bctr) {
      case 0: hostname = argument; break;
      case 1:     port = argument; break;
      default: 
        sentinel("Bad argument: '%s'", argument);
    }
    bctr++;
  }

  hostkeys = ssh_hostkeys_new();
  check_mem(hostkeys);

  session = ssh_new();
  check(session, "Error creating libssh session - '%s'", 
    ssh_get_error(session)); 

  ssh_options_set(session, SSH_OPTIONS_HOST, hostname);
  ssh_options_set(session, SSH_OPTIONS_PORT_STR, port);

  check (ssh_connect(session) == SSH_OK, "Error connecting to %s:%s - '%s'",
     hostname, port, ssh_get_error(session));
  ssh_disconnect(session);

  banners = get_banners(session);
  check(get_banners, "Error getting banners");

  printf("Issue banner: %s\n", 
    banners->issue_banner ? banners->issue_banner : "None");
  printf("Server banner: %s\n", 
    banners->server_banner ? banners->server_banner : "None");
  printf("OpenSSH version: %s\n", 
    banners->openssh_version ? banners->openssh_version 
      : "Unavailable (or server is not OpenSSH)");
  printf("Disconnect message: %s\n", 
    banners->disconnect_message ? banners->disconnect_message : "None");

  check(get_hostkey_fingerprint(session, hostkeys) == 0, 
    "Error getting hostkey fingerprints.\n");

  for (actr=1; actr< hostkeys->count; actr++) {
    hktype_name = (char *) ssh_key_type_to_char(hostkeys->keytypes[actr]);
    log_debug("Checking for key of type %s (enum value %i)", hktype_name,
      hostkeys->keytypes[actr]);

    if (hostkeys->keylengths[actr]) {
      dhash = get_display_hash(hostkeys->keyvalues[actr], 
        hostkeys->keylengths[actr], cmap);
      check(dhash, "Error getting the mapped buffer")
      printf("%s (%s)\n", dhash, hktype_name);
      free(dhash);
    }
    else {
      printf("No key of type %s\n", hktype_name);
    }
  }

  ssh_free(session);
  ssh_banners_free(banners);
  ssh_hostkeys_free(hostkeys);
  return 0;

error:
  ssh_free(session);
  ssh_banners_free(banners);
  ssh_hostkeys_free(hostkeys);
  return -1;
}

int do_input_action(
  configuration_type *config,
  composedmap_type *cmap,
  int argc, 
  char *argv[]) 
{
  int actr;
  size_t buflen;
  char *argument, *mapped_buffer=NULL;
  unsigned char *buffer=NULL;

  log_arguments_debug(argc, argv);

  check(argc >0, "No argument supplied for input action");

  for (actr=0; actr<argc; actr++) {
    argument = argv[actr];

    check(argument[0] != '-', "Bad argument: '%s'", argument);

    // Assume everything else is a hex string
    buflen = hex2buf(argument, &buffer);
    check(buflen > 0, "Could not decode input hex");

    mapped_buffer = get_display_hash(buffer, buflen, cmap);
    check_mem(mapped_buffer); 
    printf("%s => %s\n", argument, mapped_buffer);
  }

  free(mapped_buffer);
  free(buffer);
  return 0;

error:
  free(mapped_buffer);
  free(buffer);
  return -1;
}

int do_stdin_action(
  configuration_type *config,
  composedmap_type *cmap,
  int argc, 
  char *argv[]) 
{
  int chunk_max_sz=100, instring_pos, wctr;
  size_t chunk_sz;
  char inchunk[chunk_max_sz], *instring=NULL, *instring_new=NULL, 
    *input_argv[1];
  
  log_arguments_debug(argc, argv);

  check(argc == 0, "Too many arguments");

  instring_pos = 0;
  instring = malloc( sizeof(char) * chunk_max_sz);

  wctr = 0;
  while (fgets(inchunk, chunk_max_sz, stdin)) {
    log_debug("Iteration %i... Read chunk from stdin: '%s'\n", 
      wctr++, inchunk);

    chunk_sz = strlen(inchunk);
    instring_new = realloc(instring, strlen(instring) + chunk_sz + 1);
    check_mem(instring)
    instring = instring_new;
    memcpy(instring +instring_pos, inchunk, chunk_sz);
    instring_pos += chunk_sz;

    if (strlen((char*)inchunk) < (chunk_max_sz -1)) {
      log_debug("strlen(inchunk) (%zi) < chunk_max_sz -1 (%i)\n", 
        strlen((char*)inchunk), chunk_max_sz -1);
      break;
    }
  }

  log_debug("Found input '%s'", instring);

  input_argv[0] = instring;
  return do_input_action(config, cmap, 1, input_argv);

error:
  free(instring);
  free(instring_new);
  return -1;
}

int do_map_action(
  configuration_type *config,
  composedmap_type *cmap,
  int argc, 
  char *argv[]) 
{
  int ix;
  unsigned char *buffer=NULL;
  char *mapped_buffer=NULL;

  check(argc == 0, "Too many arguments");

  print_configuration_type(config, 1);

  buffer = malloc(sizeof(unsigned char) * 256);
  check_mem(buffer);
  for (ix=1; ix<256; ix++) {
    buffer[ix] = ix;
  }

  mapped_buffer = get_display_hash(buffer, 256, cmap);
  check(mapped_buffer != NULL, "Error getting display hash");

  printf("%s\n", mapped_buffer);

  free(buffer);
  free(mapped_buffer);
  return 0;

error:
  free(buffer);
  free(mapped_buffer);
  return -1;
}

int do_help_action(
  configuration_type *config,
  composedmap_type *cmap,
  int argc, 
  char *argv[]) 
{
  remembyte_help();
  return 0;
}

char *find_default_configfile() {
  int ix;
  // Note that there is *no* portable way to find the location of the currently
  // running executable in C. Naturally. 
  // So we can't do shit like ../etc/remembyte.conf here b/c that is relative 
  // to the CWD of the calling proc. 
  char *cflocs[] = {
    "~/.remembyte.conf",
    INSTALL_PREFIX"/etc/remembyte.conf",
    "./remembyte.conf",
    "./modules/remembyte.conf", // useful for bin/remembyte in checkout dir
    NULL
  };
  char *found_file = NULL;
  for (ix=0; cflocs[ix] != NULL; ix++) {
    found_file = resolve_path(cflocs[ix]);
    if (found_file) {
      log_debug("Found default configuration file at '%s'", found_file);
      return found_file;
    }
  }
  log_debug("Did not find a default config file on the filesystem\n");
  return NULL;
}

/* Parse arguments sent to remembyte
 *
 */
action_type *remembyte_optparse(
  int argc, 
  char *argv[],
  configuration_type **config)
{
  log_arguments_debug(argc, argv);

  action_type *action = action_new();
  action->func = do_help_action;

  int actr;
  char *argument, *mapname=NULL, *configfile=NULL;

  configfile = find_default_configfile();
  check(configfile, "Could not find config file");

  // Determine what action the user has specified
  for (actr=1, action->argc=0; actr<argc; actr++) {
    argument = argv[actr];

    if (*action->func == do_help_action) {
      if (safe_strcmp(argument, "stdin")) {
        action->func = do_stdin_action;
      }
      else if (safe_strcmp(argument, "input")) {
        action->func = do_input_action;
      }
      else if (safe_strcmp(argument, "ssh")) {
        action->func = do_ssh_action;
      }
      else if (safe_strcmp(argument, "map")) {
        action->func = do_map_action;
      }

      // if we just set "action", this arg has been processed
      if (*action->func != do_help_action) {
        continue;
      }
    }

    // Apply global flags
    if (strlen(argument) == 2 && argument[0] == '-') { 
      if (argument[1] == 'h') {
        remembyte_help();
        exit(0);
      }
      else if (argument[1] == 'm') {
        mapname = strdup(argv[actr+1]);
        actr++;
        continue;
      }
      else if (argument[1] == 'F') {
        configfile = resolve_path(argv[actr+1]);
        actr++;
        continue;
      }
    }

    // Any argument not parsed above goes into the action struct's argv[] array
    action->argc += 1;
    action->argv = realloc(action->argv, sizeof(char*) * action->argc );
    check_mem(action->argv);
    action->argv[ action->argc -1] = argument;
  }

  *config = process_configfile(configfile);
  check(*config, "Could not load config file from '%s'", configfile);

  if (mapname) {
    action->cmap = a2composedmap_type(mapname, *config);
    check(action->cmap, "No such mapping name '%s'", mapname);
  }
  else {
    action->cmap = (*config)->defaultmap;
    check(action->cmap, "No map name provided on command line, and no "
        "default mapping provided in config file.");
  }
  
  free(mapname);
  log_debug("Using byte mapping '%s'", action->cmap->name);
  
  return action;

error: 
  action_free(action);
  free(mapname);
  return NULL;
}

int main(int argc, char *argv[]) {
  action_type *action;
  int actionret;
  configuration_type *configuration;

  ARGV0 = argv[0];

  action = remembyte_optparse(argc, argv, &configuration);
  check(action, "Error parsing command line options");
  
  actionret = action->func(configuration, action->cmap, action->argc, action->argv);
  action_free(action);
  return actionret;

error:
  action_free(action);
  return -1;
}

