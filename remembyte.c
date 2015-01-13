#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <libssh/libssh.h>

#include "bytemaps.h"
#include "act_ssh.h"
#include "util.h"

char *ARGV0;
bool DEBUGMODE; // extern defined in util.h

typedef struct action_struct {
  int argc;
  char **argv;
  composedmap_type *cmap;
  int (*func) (configuration_type*, composedmap_type*, int, char*[]);
} action_type;

action_type *action_new() {
  action_type *action = malloc(sizeof(action_type));
  action->argc = 0;
  action->argv = NULL;
  action->cmap = composedmap_new();
  action->func = NULL;
  return action;
}

void remembyte_help() {
  printf("%s: experiments in SSH key fingerprint display\n", ARGV0);
  printf("%s [-mFDh] [SUBCOMMAND] [SUBCOMMAND OPTIONS]\n", ARGV0);
  printf("    -m MAPNAME. Maps are defined in the config file\n");
  printf("    -F CONFIGFILE. Default is ~/.remembyte.conf\n");
  printf("    -D: enable debug mode\n");
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

/* Print a string to stderr, optionally print remembyte help, and exit
 */
void exprintf(int exitcode, bool showhelp, const char * format, ...) {
  va_list args;

  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);

  if (showhelp) {
    remembyte_help();
  }

  exit(exitcode);
}

int do_ssh_action(
  configuration_type *config,
  composedmap_type *cmap,
  int argc, 
  char *argv[]) 
{
  int actr, bctr;
  char *hostname, *port, *dhash;

  dbgprintf("do_ssh_action(): argc: '%i'\n", argc);
  for (actr=0; actr<argc; actr++) {
    dbgprintf("do_ssh_action(): argv[%i]: %s\n", actr, argv[actr]);
  }

  // Set defaults...
  hostname = "localhost";
  port = "22";

  for (actr=0, bctr=0; actr<argc; actr++) {
    if (argv[actr][0] == '-') {
      fprintf(stderr, "do_ssh_action(): Bad argument - '%s'\n", argv[actr]);
      return -1;
    }
    switch (bctr) {
      case 0: hostname = argv[actr]; break;
      case 1:     port = argv[actr]; break;
      default: 
        fprintf(stderr, "do_ssh_action(): Bad argument - '%s'\n", 
        argv[actr]); 
        return -1;
    }
    bctr++;
  }

  ssh_hostkeys hostkeys = ssh_hostkeys_new();
  ssh_banners banners;

  ssh_session session;
  session = ssh_new();
  if (session == NULL) {
    fprintf(stderr, "Error creating libssh session - '%s'\n",
      ssh_get_error(session));
    return -1;
  }

  ssh_options_set(session, SSH_OPTIONS_HOST, hostname);
  ssh_options_set(session, SSH_OPTIONS_PORT_STR, port);

  // Test the connection & exit if it fails:
  if (ssh_connect(session) != SSH_OK) {
    fprintf(stderr, "Error connecting to %s:%s - %s\n",
      hostname, port, ssh_get_error(session));
    return -1;
  }
  ssh_disconnect(session);

  // Print SSH banners:
  if (get_banners(session, &banners) != 0) {
    fprintf(stderr, "Error getting banners.\n");
    return -1;
  }

  if (banners.issue_banner) {
    printf("Issue banner:\n%s\n", banners.issue_banner);
  }
  else {
    printf("No issue banner.\n");
  }

  if (banners.server_banner && strlen(banners.server_banner) >0) {
    printf("Server banner: %s.\n", banners.server_banner);
  }
  else {
    printf("No server banner.\n");
  }

  if (banners.openssh_version) {
    printf("OpenSSH version: %i.\n", banners.openssh_version);
  }
  else {
    printf("OpenSSH version: unavailable (or server is not OpenSSH).\n");
  }

  if (banners.disconnect_message) {
    printf("Disconnect message: %s.\n", banners.disconnect_message);
  }
  else {
    printf("No disconnect message.\n");
  }

  // Print SSH host key fingerprints: 
  if (get_hostkey_fingerprint(session, &hostkeys) != 0) {
    fprintf(stderr, "Error getting hostkey fingerprints.\n");
    return -1;
  }

  for (actr=0; actr< hostkeys.count; actr++) {
    if (hostkeys.keylengths[actr]) {
      dhash = get_display_hash(hostkeys.keyvalues[actr], 
        hostkeys.keylengths[actr], cmap);
      if (!dhash) {
        fprintf(stderr, "Error getting mapped buffer.\n");
        return false;
      }
      printf("%s (%s)\n", dhash, hostkeys.keytypes[actr]);
      free(dhash);
    }
    else {
      printf("No key of type %s.\n", hostkeys.keytypes[actr]);
    }
  }

  return 0;
}

int do_input_action(
  configuration_type *config,
  composedmap_type *cmap,
  int argc, 
  char *argv[]) 
{
  int actr;
  size_t buflen;
  char *hexbuf, *mapped_buffer;
  unsigned char *buffer;

  dbgprintf("do_input_action(): argc: '%i'\n", argc);
  for (actr=0; actr<argc; actr++) {
    dbgprintf("do_input_action(): argv[%i]: %s\n", actr, argv[actr]);
  }

  if (argc == 0) {
    exprintf(-1, true, "No argument supplied for INPUT action.\n");
  }

  for (actr=0; actr<argc; actr++) {
    hexbuf = argv[actr];
    // Make sure the user didn't pass us any extra options:
    if (hexbuf[0] == '-') {
      fprintf(stderr, "do_input_action(): Bad argument - '%s'\n", hexbuf);
      return -1;
    }

    // Assume everything else is a hex string
    buflen = hex2buf(hexbuf, &buffer);
    if (buflen <= 0) {
      fprintf(stderr, "ERROR: Could not decode input hex.\n");
      return -1;
    }

    mapped_buffer = get_display_hash(buffer, buflen, cmap);
    if (!mapped_buffer) {
      exprintf(-1, false, "Failed to map buffer '%s'.\n", hexbuf);
    }
    printf("%s => %s\n", hexbuf, mapped_buffer);
  }

  return 0;
}

int do_stdin_action(
  configuration_type *config,
  composedmap_type *cmap,
  int argc, 
  char *argv[]) 
{
  int chunk_max_sz=100, instring_pos, actr, wctr;
  size_t chunk_sz;
  //char *inchunk[chunk_max_sz], *instring, *instring_new,
  //  *input_argv[1];
  char *inchunk[chunk_max_sz], *instring, *instring_new, *input_argv[1];
  

  dbgprintf("do_stdin_action(): argc: '%i'\n", argc);
  for (actr=0; actr<argc; actr++) {
    dbgprintf("do_stdin_action(): argv[%i]: %s\n", actr, argv[actr]);
  }

  if (argc != 0) {
    fprintf(stderr, "do_stdin_action(): too many arguments\n");
    return -1;
  }

  instring_pos = 0;
  instring = malloc( sizeof(char) * chunk_max_sz);

  wctr = 0;
  while (fgets((char*)inchunk, chunk_max_sz, stdin)) {
    dbgprintf("Iteration %i... Read chunk from stdin: '%s'\n", wctr++, inchunk);

    chunk_sz = strlen((const char *)inchunk);
    instring_new = realloc(instring, strlen(instring) + chunk_sz + 1);
    if (!instring) {
      fprintf(stderr, "Could not allocate memory\n");
      free(instring);
      return -1;
    }
    instring = instring_new;
    memcpy(instring +instring_pos, inchunk, chunk_sz);
    instring_pos += chunk_sz;

    if (strlen((char*)inchunk) < (chunk_max_sz -1)) {
      dbgprintf("strlen(inchunk) (%i)   <   chunk_max_sz -1 (%i)\n", 
        strlen((char*)inchunk), chunk_max_sz -1);
      break;
    }
  }

  dbgprintf("do_stdin_action() input: '%s'\n", instring);

  input_argv[0] = instring;
  return do_input_action(config, cmap, 1, input_argv);
}

int do_map_action(
  configuration_type *config,
  composedmap_type *cmap,
  int argc, 
  char *argv[]) 
{
  int ix;
  unsigned char * buffer;
  char * mapped_buffer;

  if (argc != 0) {
    fprintf(stderr, "do_map_action(): too many arguments\n");
    return -1;
  }

  print_configuration_type(config, 1);

  buffer = malloc(sizeof(unsigned char) * 256);
  for (ix=1; ix<256; ix++) {
    buffer[ix] = ix;
  }

  mapped_buffer = get_display_hash(buffer, 256, cmap);
  if (!mapped_buffer) {
    exprintf(-1, false, "Failed to map buffer.\n");
  }
  printf("%s\n", mapped_buffer);

  free(mapped_buffer);
  free(buffer);
  return 0;
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

#define CFLOCS_SZ 3
char *find_default_configfile() {
  int ix;
  char *cflocs[CFLOCS_SZ] = {
    "~/.remembyte.conf", "../etc/remembyte.conf", "./remembyte.conf"
  };
  char *found_file = NULL;
  for (ix=0; ix<CFLOCS_SZ; ix++) {
    found_file = resolve_path(cflocs[ix]);
    if (found_file) {
      dbgprintf("Found default configuration file at '%s'\n", found_file);
      return found_file;
    }
  }
  dbgprintf("Did not find a default config file on the filesystem\n");
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
/*  
void remembyte_optparse(
  int argc, 
  char *argv[], 
  configuration_type *config,
  composedmap_type *cmap,
  void **action, 
  int *action_argc, // TODO: require this to be malloc'd by the caller
  char **action_argv)  // TODO: require this to be malloc'd by the caller
{
*/

  action_type *action = action_new();
  action->func = do_help_action;

  int actr;
  char *argument, *mapname, *configfile;

  mapname = NULL;
  configfile = find_default_configfile();

  dbgprintf("remembyte_optparse(): argc == %i\n", argc);

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
      else if (argument[1] == 'D') {
        DEBUGMODE=true;
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
    action->argv[ action->argc -1] = argument;
  }

  *config = process_configfile(configfile);
  if (!*config) {
    exprintf(-1, false, "Could not load config file from '%s'.\n", configfile);
  }

  if (mapname) {
    action->cmap = a2composedmap_type(mapname, *config);
    if (!action->cmap) {
      exprintf(-1, false, "No such mapping name '%s'\n", mapname);
    }
  }
  else {
    action->cmap = get_default_map(*config);
    if (!action->cmap) {
      exprintf(-1, false, "No map name provided on command line, and no "
        "default mapping provided in config file.\n");
    }
  }
  
  free(mapname);
  dbgprintf("Using byte mapping '%s'\n", action->cmap->name);

  for(actr=0; actr< action->argc; actr++) {
    dbgprintf("action->argv[%i] == %s\n", actr, action->argv[actr]);
  }
  
  return action;
}

int main(int argc, char *argv[]) {
  /*
  int (*action)(composedmap_type*, int, char**);
  int *action_argc;
  char **action_argv;
   */
  action_type *action;
  int actionret;
  configuration_type *configuration;
  composedmap_type cmap;

  ARGV0 = argv[0];
  DEBUGMODE = false;

  /*
  remembyte_optparse(argc, argv, configuration, &cmap, &(action),
    action_argc, action_argv);
  */
  action = remembyte_optparse(argc, argv, &configuration);
  
  actionret = action->func(configuration, action->cmap, action->argc, action->argv);
  return actionret;
}

