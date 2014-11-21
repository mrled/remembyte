#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include <stdarg.h>

#include <libssh/libssh.h>

#include "bytemaps.h"

char *argv0;

typedef enum {
  HEX,
  EMOJI
} dmode_t;
dmode_t displaymode;

int DEBUGMODE;

void dbgprintf(const char *format, ...) {
  if (DEBUGMODE) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
  }
}

dmode_t a2dmode(char *dmode_name) {
  dmode_t dm;
  if (strlen(dmode_name) >=3 && strncmp(dmode_name, "hex", 1) == 0) {
    dm = HEX;
  }
  else if (strlen(dmode_name) >=5 && strncmp(dmode_name, "emoji", 1) == 0) {
    dm = EMOJI;
  }
  else {
    fprintf(stderr, "No such display mode: %s\n", dmode_name);
    exit(-1);
  }
  return dm;
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

char* hexbuf2emoji_old(unsigned char *hash, size_t hash_len) {
  /* (space btwn each emoji) + (emoji are 4 wide) + (terminator \0) */ 
  char *outstring = malloc((sizeof(char)*hash_len) + (sizeof(char)*hash_len*4) +1); 
  unsigned int hashbyte;
  char emojibyte[4];
  int inctr=0, outctr=0, emoctr=0;
  for (inctr=0; inctr<hash_len; inctr++) {
    outctr = inctr*5;

    hashbyte = (unsigned int)hash[inctr];
    if (! (255 >= hashbyte >= 0)) {
      fprintf(stderr, 
              "Error: the hash buffer has a byte in it not between 0 and 256.");
      exit(-1);
    }

    emojibyte[0] = emoji_map[hashbyte];
    emojibyte[1] = emoji_map[hashbyte+1];
    emojibyte[2] = emoji_map[hashbyte+2];
    emojibyte[3] = emoji_map[hashbyte+3];
    printf("%s", emojibyte);

    outstring[outctr] = emoji_map[hashbyte];
    outstring[outctr+1] = emoji_map[hashbyte+1];
    outstring[outctr+2] = emoji_map[hashbyte+2];
    outstring[outctr+3] = emoji_map[hashbyte+3];
    outstring[outctr+4] = (char)32;
    //    dbgprintf("emojibyte == '%c'\n", emojibyte);
    //dbgprintf("hashbyte %s(%i) == emojibyte %s\n", (char)hashbyte, hashbyte, emojibyte);
    //dbgprintf("hashbyte %s", (hashbyte);
    //outstring[outctr] = emojibyte;
    //outstring[++outctr] = " ";
  }
  return outstring; 
  /* TODO: how to free this memory? */
}
wchar_t* hexbuf2emoji(unsigned char *hash, size_t hash_len) {
  wchar_t outstring[hash_len];
  unsigned int hashbyte;
  wchar_t emojibyte;
  int inctr=0;
  for (inctr=0; inctr<hash_len; inctr++) {
    hashbyte = (unsigned int)hash[inctr];

    emojibyte = emoji_map[hashbyte];
    printf("%ls", &emojibyte);

    outstring[inctr] = emojibyte;
  }
  return outstring; 
  /* TODO: how to free this memory? */
}

void get_display_hash(unsigned char *hash, size_t hash_len, char **outstring) {
  char *os;
  switch (displaymode) {
    case HEX:
      os = ssh_get_hexa(hash, hash_len); break;
    case EMOJI:
      os = hexbuf2emoji(hash, hash_len); break;
    default:
      fprintf(stderr, 
              "displaymode is set to %i but I can't tell what that means ",
              displaymode);
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

void usage() {
  printf("Usage: %s [-h hostname] [-p port]\n", argv0);
}

int main(int argc, char *argv[]) {
  char *dm_str;

  argv0 = argv[0];
  DEBUGMODE = 0;
  dm_str = "hex";
  char *hostname = "localhost";
  char *port = "22";

  int opt;
  while ((opt = getopt(argc, argv, "h:p:d:D")) != -1) {
    switch (opt) {
      case 'h': hostname = optarg; break;
      case 'p': port = optarg; break;
      case 'd': dm_str = optarg; break;
      case 'D': DEBUGMODE = 1; break;

      case '?':
      default:
        usage();
        exit(-1);
    }
  }
  argc -= optind;
  argv += optind;

  displaymode = a2dmode(dm_str);

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

