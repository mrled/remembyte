#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <libssh/libssh.h>

#include "bytemaps.h"

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

typedef enum {
  HEX,
  EMOJI
} mapping_t;
mapping_t mapping;
mapping_t a2mapping_t(char *map_name) {
  mapping_t map;
  if (strlen(map_name) >=3 && strncmp(map_name, "hex", 1) == 0) {
    map = HEX;
  }
  else if (strlen(map_name) >=5 && strncmp(map_name, "emoji", 1) == 0) {
    map = EMOJI;
  }
  else {
    fprintf(stderr, "No such bytemap: %s\n", map_name);
    exit(-1);
  }
  return map;
}

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

void connect_or_exit_w_err(ssh_session session) {
  if (ssh_connect(session) != SSH_OK) {
    char *hostname; 
    unsigned int port;
    ssh_options_get(session, SSH_OPTIONS_HOST, &hostname);
    ssh_options_get_port(session, &port);
    fprintf(stderr, "Error connecting to %s:%u - %s\n",
      hostname, port, ssh_get_error(session));
    exit(-1);
  }
}

char* concat_bytearray(char *s1, char *s2) {
    size_t len1 = strlen(s1);
    size_t len2 = strlen(s2);
    size_t outsz = len1+len2+1; // +1 for \0
    char *result = malloc(outsz); 
    if (!result) {
      fprintf(stderr, "Error allocating memory of size %zi\n", outsz);
    }

    memcpy(result, s1, len1);
    memcpy(result+len1, s2, len2+1); //+1 for \0
    return result;
  // TODO: how to free this memory? 
}

char *hexbuf2emoji(unsigned char *hash, size_t hash_len) {
  char *emojibyterep;
  int inctr, outstring_len=0;
  unsigned int hashbyte;
  char *os="";

  for (inctr=0; inctr<hash_len; inctr++) {
    hashbyte = (unsigned int)hash[inctr];
    emojibyterep = (char *)emoji_map[hashbyte];

    os = concat_bytearray(os, emojibyterep);

    // Add a space between each emoji b/c they will overlap otherwise
    // Add a colon after each emoji except the last one
    if (inctr != hash_len-1) {
      os = concat_bytearray(os, " :");
    }
    else {
      os = concat_bytearray(os, " ");
    }
  }
  return os; 
  // TODO: how to free this memory? 
}

void get_display_hash(unsigned char *hash, size_t hash_len, char **outstring) {
  char *os;
  switch (mapping) {
    case HEX:
      os = ssh_get_hexa(hash, hash_len); break;
    case EMOJI:
      os = hexbuf2emoji(hash, hash_len); break;
    default:
      fprintf(stderr, 
              "ERROR: mapping is set to %i but I can't tell what that means.\n",
              mapping);
  }
  *outstring = os;
  /* TODO: free the 'os' memory how? */
}

int get_banners(ssh_session session) {
  const char *sbanner, *dmessage;
  char *issue, *cbanner;
  int osshv;

  connect_or_exit_w_err(session);

  /* TODO: why doesn't ssh_get_issue_banner work? 
   * I can set a banner that openssh will display but this won't.
   */
  issue = ssh_get_issue_banner(session);
  if (issue) {
    printf("Issue banner:\n%s\n", issue);
  }
  else {
    printf("No issue banner.\n");
  }

  sbanner = ssh_get_serverbanner(session);
  if (sbanner) {
    printf("Server banner: %s.\n", sbanner);
  }
  else {
    printf("No server banner.\n");
  }

 
  /* TODO: parse OpenSSH version so it returns the real version.
   * I have no idea what this value is returning??
   */
  osshv = ssh_get_openssh_version(session);
  if (osshv) {
    printf("OpenSSH version: %i.\n", osshv);
  }
  else {
    printf("OpenSSH version: unavailable (or server is not OpenSSH).\n");
  }

  /* This does not actually initiate a disconnect.*/
  dmessage = ssh_get_disconnect_message(session);
  if (dmessage) {
    printf("Disconnect message: %s.\n", dmessage);
  }
  else {
    printf("No disconnect message.\n");
  }

  ssh_disconnect(session);
  return 0;
}

int get_host_key_fingerprint(ssh_session session, unsigned char **hostkeytypes, unsigned char **hostkeys, int hk_len) {

  size_t hkhash_buf_len;
  ssh_key pubkey;
  char *hexa;
  int i;

  unsigned char *hkhash_buf;
  unsigned char *hkhash_display;

  unsigned char *hkhash;

  char *hostname; 
  unsigned int port;
  ssh_options_get(session, SSH_OPTIONS_HOST, &hostname);
  ssh_options_get_port(session, &port);

  int *blockarray;
  double blockarray_len;

  for (i=0; i<hk_len; i++) {
    if (ssh_options_set(session, SSH_OPTIONS_HOSTKEYS, hostkeytypes[i]) != 0) {
      fprintf(stderr, "Error setting SSH option for host key '%s': %s\n", hostkeytypes[i], ssh_get_error(session));
      return -1;
    }

    if (ssh_connect(session) != SSH_OK) {
      printf("Server does not support key of type %s.\n", hostkeytypes[i]);
    }

    else {

      if (ssh_get_publickey(session, &pubkey) != SSH_OK) {
        fprintf(stderr, "Error getting public key: %s\n", ssh_get_error(session));
        return -1;
      }

      /* The third ("hash") argument to ssh_get_publickey_hash comes back as a 
       * character buffer of hex values. However, it's padded with zeroes. 

         (lldb) frame variable hash
         (unsigned char *) hostkey = 0x000000000119c860 "\x81\x0f\x13)\xab\xf2\xb4g\xd0\x13\x89w\x96]o\x01XXXXXXXX1"
         (lldb) frame variable ssh_get_hexa(hash, hash_len) // this is lldb pseudocode, of course
         (char *) hexa = 0x000000000119db70 "81:0f:13:29:ab:f2:b4:67:d0:13:89:77:96:5d:6f:01"

       */
      //if (ssh_get_publickey_hash(pubkey, SSH_PUBLICKEY_HASH_MD5, &hostkeys[i], &hlen) != 0) {
      if (ssh_get_publickey_hash(pubkey, SSH_PUBLICKEY_HASH_MD5, &hkhash_buf, &hkhash_buf_len) != 0) {
        fprintf(stderr, "Error getting public key hash: %s\n", ssh_get_error(session));
        return -1;
      }

      hostkeys[i] = hkhash_buf;

      char *display;
      get_display_hash(hkhash_buf, hkhash_buf_len, &display);

      printf("%s (%s)\n", display, hostkeytypes[i]);

    }

    ssh_disconnect(session);

  }

  ssh_key_free(pubkey);

  return 0;
}

int do_ssh_action(char *hostname, char *port) {
  int hk_len = 3;
  unsigned char *hostkeytypes[hk_len];
  hostkeytypes[0] = (unsigned char *)"ecdsa-sha2-nistp256";
  hostkeytypes[1] = (unsigned char *)"ssh-dss"; 
  hostkeytypes[2] = (unsigned char *)"ssh-rsa";

  unsigned char *hostkeys[hk_len];
  int i;
  for (i=0; i<hk_len; i++) {
    /* hex representations of MD5 hashes are 32 characters */
    hostkeys[i] = malloc(32*sizeof(unsigned char));
  }

  ssh_session session;
  session = ssh_new();
  if (session == NULL) return -1;

  ssh_options_set(session, SSH_OPTIONS_HOST, hostname);
  ssh_options_set(session, SSH_OPTIONS_PORT_STR, port);

  get_banners(session);
  get_host_key_fingerprint(session, hostkeytypes, hostkeys, hk_len);

  return 0;
}

int do_map_action() {
  int ctr;
  switch (mapping) {
    case HEX:
      printf("HEX mapping:\nThis should be self-explanatory, man.\n"); break;
    case EMOJI:
      printf("EMOJI mapping: \n");
      for (ctr=0; ctr<emoji_map_len; ctr++){
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
  printf("    Arguments: \n");
  printf("    -h HOSTNAME. Defaults to localhost.\n");
  printf("    -p PORTNUM. Defaults to 22\n");
  printf("    -m MAPPING. Possible values: hex (default), emoji.\n");
  printf("    -a ACTION. Possible values: ssh (default), map.\n");
  printf("        ssh: connect to an ssh server.\n");
  printf("        map: display the mapping specified by -m.\n");
  printf("    -D: enable debug mode.\n");
}

int main(int argc, char *argv[]) {
  char *map_str, *action_str;

  argv0 = argv[0];
  DEBUGMODE = 0;
  map_str = "hex";
  action_str = "ssh";
  char *hostname = "localhost";
  char *port = "22";

  int opt;
  while ((opt = getopt(argc, argv, "h:p:m:a:D")) != -1) {
    switch (opt) {
      case 'h': hostname = optarg; break;
      case 'p': port = optarg; break;
      case 'm': map_str = optarg; break;
      case 'a': action_str = optarg; break;
      case 'D': DEBUGMODE = 1; break;

      case '?':
      default:
        emossh_help();
        exit(-1);
    }
  }
  argc -= optind;
  argv += optind;

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

