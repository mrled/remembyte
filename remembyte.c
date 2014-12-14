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
  printf("        arguments: INPUTSTRING [INPUTSTRING [...]] (as hex)\n");
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
  int actr, bctr;
  char * hostname, * port;

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
  int actr, buflen;
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

    mapped_buffer = get_display_hash(buffer, buflen, mapping);
    printf("%s => %s\n", hexbuf, mapped_buffer);
  }

  return 0;
}

int do_map_action(int argc, char *argv[]) {
  int ix, actr;
  unsigned char * buffer;
  char * mapped_buffer;

  if (argc != 0) {
    fprintf(stderr, "do_map_action(): too many arguments\n");
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
 *
 * @return an action_type struct that contains a function pointer and arguments
 * to call
 */
action_type remembyte_optparse(int argc, char *argv[]) {
  action_type action;
  int actr;
  char * argument;

  action.func = NULL;
  action.argv = malloc(sizeof(char*) * argc); // this is too big. oh well.

  dbgprintf("remembyte_optparse(): argc == %i\n", argc);

  // Determine what action the user has specified
  for (actr=1, action.argc=0; actr<argc; actr++) {
    argument = argv[actr];

    if (!action.func) {
      if (safe_strcmp(argument, "input")) {
        action.func = &do_input_action;
      }
      else if (safe_strcmp(argument, "ssh")) {
        action.func = &do_ssh_action;
      }
      else if (safe_strcmp(argument, "map")) {
        action.func = &do_map_action;
      }

      // if we just set action.func, this arg has been processed
      if (action.func) {
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
        mapping = a2mapping_t(argv[actr+1]);
        actr++;
        continue;
      }
      else if (argument[1] == 'D') {
        DEBUGMODE=true;
        continue;
      }
    }

    // Any argument not parsed above goes into the action struct's argv[] array
    action.argv[action.argc] = argument;
    action.argc++;
  }

  // The default action is the input action
  if (!action.func) {
    action.func = &do_input_action;
  }

  if (DEBUGMODE) {
    for(actr=0; actr<action.argc; actr++) {
      dbgprintf("action.argv[%i] == %s\n", actr, action.argv[actr]);
    }
  }

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

