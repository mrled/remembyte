#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <libssh/libssh.h>

#include "bytemaps.h"
#include "act_ssh.h"
#include "util.h"

char *argv0;
extern bool DEBUGMODE;

mapping_t mapping;

/* A struct we will use to keep track of actions and their arguments
 */
typedef struct {
  enum { UNSET, INPUT, SSH, MAP } code;
  int (* func) (int, char*[]);
  int argc;
  char ** argv;
} action_type;


void remembyte_help() {
  printf("%s: experiments in SSH key fingerprint display\n", argv0);
  printf("%s [-mDh] [SUBCOMMAND] [SUBCOMMAND OPTIONS]\n", argv0);
  printf("    -m MAPPING. Possible values: hex (default), emoji, pgp\n");
  printf("    -D: enable debug mode\n");
  printf("    -h: display this message\n");
  printf("    SUBCOMMAND: one of the following:\n");
  printf("     -  ssh: connects to an SSH server and prints a key fingerprint\n");
  printf("        arguments: [HOSTNAME] [PORT]\n");
  printf("     -  map: prints the selected map from 0x00-0xff\n");
  printf("        arguments: none\n");
  printf("     -  input: takes input on the command line\n");
  printf("        arguments: INPUTSTRING (as hex)\n");
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

int do_ssh_action(int argc, char *argv[]) {
  int actr, pctr; // mnemonic: argument counter, parsed argument counter
  char * hostname, * port;

  dbgprintf("do_ssh_action(): argc: '%i'\n", argc);
  for (actr=0; actr<argc; actr++) {
    dbgprintf("do_ssh_action(): argv[%i]: %s\n", actr, argv[actr]);
  }

  // Set defaults...
  hostname = "localhost";
  port = "22";

  for (actr=0, pctr=0; actr<argc; actr++) {
    if (argv[actr][0] == '-') {
      fprintf(stderr, "do_ssh_action(): Bad argument - %s\n", argv[actr]);
      return -1;
    }
    switch (pctr) {
      case 0: hostname = argv[actr]; break;
      case 1:     port = argv[actr]; break;
      default: 
        fprintf(stderr, "do_ssh_action(): Bad argument - %s\n", 
        argv[actr]); 
        return -1;
    }
    pctr++;
  }

  ssh_hostkeys hostkeys = ssh_hostkeys_new();
  ssh_banners banners;

  ssh_session session;
  session = ssh_new();
  if (session == NULL) {
    fprintf(stderr, "Error creating libssh session - %s\n",
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

  // TODO: do return checking here?
  if (get_banners(session, &banners) != 0) {
    fprintf(stderr, "Error getting banners.\n");
    return -1;
  }
  print_banners(&banners);

  if (get_hostkey_fingerprint(session, &hostkeys) != 0) {
    fprintf(stderr, "Error getting hostkey fingerprints.\n");
    return -1;
  }
  print_hostkey_fingerprint(&hostkeys, mapping);
  return 0;
}

int do_input_action(int argc, char *argv[]) {

  int actr;
  char *hexbuf;

  dbgprintf("do_input_action(): argc: '%i'\n", argc);
  for (actr=0; actr<argc; actr++) {
    dbgprintf("do_input_action(): argv[%i]: %s\n", actr, argv[actr]);
  }

  if (argc == 0) {
    exprintf(-1, true, "No argument supplied for INPUT action.\n");
  }

  for (actr=0; actr<argc; actr++) {
    if (argv[actr][0] == '-') {
      fprintf(stderr, "do_input_action(): Bad argument - '%s'\n", argv[actr]);
      return -1;
    }
    switch (actr) {
      case 0: hexbuf = argv[actr]; break;
      default: 
        fprintf(stderr, "do_input_action(): Bad argument - '%s'\n", 
        argv[actr]); 
        return -1;
    }
  }

  unsigned char * buffer;
  char * mapped_buffer;
  int buflen;

  buflen = hex2buf(hexbuf, &buffer);
  if (buflen <= 0) {
    fprintf(stderr, "ERROR: Could not decode input hex.\n");
    return -1;
  }

  mapped_buffer = get_display_hash(buffer, buflen, mapping);

  printf("Input value: \n%s\n", hexbuf);
  printf("Maps to: \n%s\n", mapped_buffer);

  return 0;
}

int do_map_action(int argc, char *argv[]) {
  int ix, actr;
  unsigned char * buffer;
  char * mapped_buffer;

  if (argc != 0) {
    fprintf(stderr, "do_map_action(): too many arguments\n", argv[0]);
    return -1;
  }

  buffer = malloc(sizeof(unsigned char *) * 256);
  for (ix=1; ix<256; ix++) {
    buffer[ix] = ix;
  }

  mapped_buffer = get_display_hash(buffer, 256, mapping);
  printf("%s\n", mapped_buffer);

  free(mapped_buffer);
  free(buffer);
  return 0;
}

/* Parse arguments sent to remembyte
 */
action_type remembyte_optparse(int argc, char *argv[]) {
  action_type action;
  int actr, pctr, bctr;
  char * argument;

  action.code = UNSET;
  char ** act_argv_prime = malloc(sizeof(char*) * argc); // this is too big. oh well.
  char ** act_argv = malloc(sizeof(char*) * argc); // this is too big. oh well.
  int act_argc;

  dbgprintf("remembyte_optparse(): argc == %i\n", argc);

  // Determine what action the user has specified
  // TODO: pretty sure I can get rid of action.code here altogether
  bool setit;
  for (actr=1, bctr=0; actr<argc; actr++) {
    setit=false;
    argument = argv[actr];

    if (action.code == UNSET) {
      if (safe_strcmp(argument, "input")) {
        action.code = INPUT;
        action.func = &do_input_action;
        setit=true;
      }
      else if (safe_strcmp(argument, "ssh")) {
        action.code = SSH;
        action.func = &do_ssh_action;
        setit=true;
      }
      else if (safe_strcmp(argument, "map")) {
        action.code = MAP;
        action.func = &do_map_action;
        setit=true;
      }
    }

    if (!setit){
      act_argv_prime[bctr] = argv[actr];
      bctr++;
    }
  }
  // The default action is the input action
  if (action.code == UNSET) {
    action.code = INPUT;
  }

  // Apply global flags, and gather action-specific arguments into the action
  // struct's argv[] array.
  for (actr=0, act_argc=0; actr<bctr; actr++) {
    argument = act_argv_prime[actr];

    if (strlen(argument) == 2 && argument[0] == '-') { // get global flags
      switch (argument[1]) {
        case 'm':
          mapping = a2mapping_t(act_argv_prime[actr+1]);
          actr++;
          break;
        case 'D':
          DEBUGMODE=true;
          break;
        case 'h':
          remembyte_help();
          exit(0);
      }
      continue;
    }

    act_argv[act_argc] = argument;
    act_argc++; // do NOT increment this in the parents of the for() loop
  }

  if (DEBUGMODE) {
    for(actr=0; actr<act_argc; actr++) {
      dbgprintf("act_argv[%i] == %s\n", actr, act_argv[actr]);
    }
  }

  action.argv = act_argv;
  action.argc = act_argc;
  return action;
}

int main(int argc, char *argv[]) {
  action_type action;
  int actionret;

  argv0 = argv[0];
  DEBUGMODE = false;

  action = remembyte_optparse(argc, argv);
  actionret = action.func(action.argc, action.argv);
  return actionret;
}

