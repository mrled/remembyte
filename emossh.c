#include "emossh.h"

char *argv0;
int DEBUGMODE;

void dbgprintf(const char *format, ...) {
  if (DEBUGMODE) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
  }
}

mapping_t mapping;

typedef enum {
  SSH,
  MAP
} action_t;
action_t action;
action_t a2action_t(char *action_name) {
  action_t act;
  if (strlen(action_name) >=3 && strncmp(action_name, "ssh", 1) == 0) {
    act = SSH;
  }
  else if (strlen(action_name) >=3 && strncmp(action_name, "map", 1) == 0) {
    act = MAP;
  }
  else {
    fprintf(stderr, "No such action: %s\n", action_name);
    exit(-1);
  }
  return act;
}

int do_ssh_action(char *hostname, char *port) {
  ssh_hostkeys hostkeys = ssh_hostkeys_new();
  ssh_banners banners;

  ssh_session session;
  session = ssh_new();
  if (session == NULL) return -1;

  ssh_options_set(session, SSH_OPTIONS_HOST, hostname);
  ssh_options_set(session, SSH_OPTIONS_PORT_STR, port);

  // Test the connection & exit if it fails:
  if (ssh_connect(session) != SSH_OK) {
    fprintf(stderr, "Error connecting to %s:%s - %s\n",
      hostname, port, ssh_get_error(session));
    exit(-1);
  }
  ssh_disconnect(session);

  // TODO: do return checking here?
  if (get_banners(session, &banners) != 0) {
    fprintf(stderr, "Error getting banners.\n");
    exit(-1);
  }
  print_banners(&banners);

  if (get_hostkey_fingerprint(session, &hostkeys) != 0) {
    fprintf(stderr, "Error getting hostkey fingerprints.\n");
    exit(-1);
  }
  print_hostkey_fingerprint(&hostkeys, mapping);

  return 0;
}

int do_map_action() {
  int ctr;
  switch (mapping) {
    case HEX:
      printf("HEX mapping:\nThis should be self-explanatory, man.\n"); break;
    case EMOJI:
      printf("EMOJI mapping: \n");
      for (ctr=0; ctr<EMOJI_MAP_LEN; ctr++){
        printf("%s , ", emoji_map[ctr]);
      }
      printf("\n");
      break;
    default:
      fprintf(stderr, 
              "ERROR: mapping is set to %i but I can't tell what that means.\n",
              mapping);
  }
  /* TODO: free the 'os' memory how? */
  return 0;
}

void emossh_help() {
  printf("%s: experiments in SSH key fingerprint display.\n", argv0);
  printf("%s [-maDh] [HOSTNAME [PORT]]\n", argv0);
  printf("    HOSTNAME: the host to connect to. Defaults to localhost.\n");
  printf("    PORT: the port to connect to. Defaults to 22.\n");
  printf("    -m MAPPING. Possible values: hex (default), emoji.\n");
  printf("    -a ACTION. Possible values: ssh (default), map.\n");
  printf("        ssh: connect to an ssh server.\n");
  printf("        map: display the mapping specified by -m.\n");
  printf("    -D: enable debug mode.\n");
  printf("    -h: display this message.\n");
}

int main(int argc, char *argv[]) {
  char *map_str, *action_str;

  argv0 = argv[0];
  DEBUGMODE = 0;
  map_str = "hex";
  action_str = "ssh";
  char *hostname, *port;

  int opt;
  while ((opt = getopt(argc, argv, "m:a:Dh")) != -1) {
    switch (opt) {
      case 'm': map_str = optarg; break;
      case 'a': action_str = optarg; break;
      case 'D': DEBUGMODE = 1; break;

      case 'h':
      default:
        emossh_help();
        exit(-1);
    }
  }
  argc -= optind;
  argv += optind;

  dbgprintf("argc = %i, optind = %i\n", argc, optind);

  hostname = "localhost"; 
  port = "22";

  optind = 0; 
  if (argc > optind) {
    hostname = argv[optind];
    optind++;
  }
  if (argc > optind) {
    port = argv[optind];
    optind++;
  }
  if (argc > optind) {
    fprintf(stderr, "ERROR: too many positional arguments.\n");
    emossh_help();
    exit(-1);
  }

  dbgprintf("Using hostname '%s' and port '%s'\n", hostname, port);

  mapping = a2mapping_t(map_str);
  action = a2action_t(action_str);

  switch (action) {
    case SSH:
      return do_ssh_action(hostname, port); break;
    case MAP:
      return do_map_action(); break;
    default:
      fprintf(stderr, 
              "action is set to %i but I can't tell what that means ",
              action);
  }
}

