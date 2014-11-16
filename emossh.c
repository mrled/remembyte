#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libssh/libssh.h>

const char *emojis = "🌀🌂🌅🌈🌙🌞🌟🌠🌰🌱🌲🌳🌴🌵🌷🌸🌹🌺🌻🌼🌽🌾🌿🍀🍁🍂🍃🍄🍅🍆🍇🍈🍉🍊🍋🍌🍍🍎🍏🍐🍑🍒🍓🍔🍕🍖🍗🍘🍜🍝🍞🍟🍠🍡🍢🍣🍤🍥🍦🍧🍨🍩🍪🍫🍬🍭🍮🍯🍰🍱🍲🍳🍴🍵🍶🍷🍸🍹🍺🍻🍼🎀🎁🎂🎃🎄🎅🎈🎉🎊🎋🎌🎍🎎🎏🎒🎓🎠🎡🎢🎣🎤🎥🎦🎧🎨🎩🎪🎫🎬🎭🎮🎯🎰🎱🎲🎳🎴🎵🎷🎸🎹🎺🎻🎽🎾🎿🏀🏁🏂🏃🏄🏆🏇🏈🏉🏊🐀🐁🐂🐃🐄🐅🐆🐇🐈🐉🐊🐋🐌🐍🐎🐏🐐🐑🐒🐓🐔🐕🐖🐗🐘🐙🐚🐛🐜🐝🐞🐟🐠🐡🐢🐣🐤🐥🐦🐧🐨🐩🐪🐫🐬🐭🐮🐯🐰🐱🐲🐳🐴🐵🐶🐷🐸🐹🐺🐻🐼🐽🐾👀👂👃👄👅👆👇👈👉👊👋👌👍👎👏👐👑👒👓👔👕👖👗👘👙👚👛👜👝👞👟👠👡👢👣👤👥👦👧👨👩👪👮👯👺👻👼👽👾👿💀💁💂💃💄💅";

char *argv0;

int get_host_key_fingerprint(ssh_session session, unsigned char **hostkeytypes, unsigned char **hostkeys, int hk_len) {

  size_t hlen;
  ssh_key pubkey;
  char *hexa;
  int i;

  for (i=0; i<hk_len; i++) {
    if (ssh_options_set(session, SSH_OPTIONS_HOSTKEYS, hostkeytypes[i]) != 0) {
      fprintf(stderr, "Error setting SSH option for host key '%s': %s\n", hostkeytypes[i], ssh_get_error(session));
      return -1;
    }

    if (ssh_connect(session) != SSH_OK) {
      fprintf(stderr, "Error connecting to localhost: %s\n", ssh_get_error(session));
      return -1;
    }

    if (ssh_get_publickey(session, &pubkey) != SSH_OK) {
      fprintf(stderr, "Error getting public key: %s\n", ssh_get_error(session));
      return -1;
    }

    if (ssh_get_publickey_hash(pubkey, SSH_PUBLICKEY_HASH_MD5, &hostkeys[i], &hlen) != 0) {
      fprintf(stderr, "Error getting public key hash: %s\n", ssh_get_error(session));
      return -1;
    }

    ssh_disconnect(session);

    hexa = ssh_get_hexa(hostkeys[i], hlen);
    printf("%s (%s)\n", hexa, hostkeytypes[i]);
  }

  ssh_key_free(pubkey);

  return 0;
}

void usage() {
  printf("Usage: %s [-h hostname] [-p port]\n", argv0);
}

int main(int argc, char *argv[]) {
  argv0 = argv[0];
  char *hostname = "localhost";
  /*unsigned int port = 22;*/
  char *port = "22";

  int opt;
  while ((opt = getopt(argc, argv, "h:p:")) != -1) {
    switch (opt) {
      case 'h': hostname = optarg; break;
      case 'p': port = optarg; break;

      case '?':
      default:
        usage();
        exit(-1);
    }
  }
  argc -= optind;
  argv += optind;

  /*
  int hk_len = 3;
  unsigned char *hostkeytypes[hk_len];
  hostkeytypes[0] = "ecdsa-sha2-nistp256";
  hostkeytypes[1] = "ssh-dss"; 
  hostkeytypes[2] = "ssh-rsa";
  */
  int hk_len = 2;
  unsigned char *hostkeytypes[hk_len];
  hostkeytypes[0] = "ssh-rsa";
  hostkeytypes[1] = "ssh-dss"; 
  
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

  get_host_key_fingerprint(session, hostkeytypes, hostkeys, hk_len);

  /*    hexa = ssh_get_hexa(hostkeys[i], );
        printf("%s (%s)\n", hexa, hostkeytypes[i]);*/

  return 0;
}

