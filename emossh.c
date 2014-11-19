#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libssh/libssh.h>

const char *emojis = "🌀🌂🌅🌈🌙🌞🌟🌠🌰🌱🌲🌳🌴🌵🌷🌸🌹🌺🌻🌼🌽🌾🌿🍀🍁🍂🍃🍄🍅🍆🍇🍈🍉🍊🍋🍌🍍🍎🍏🍐🍑🍒🍓🍔🍕🍖🍗🍘🍜🍝🍞🍟🍠🍡🍢🍣🍤🍥🍦🍧🍨🍩🍪🍫🍬🍭🍮🍯🍰🍱🍲🍳🍴🍵🍶🍷🍸🍹🍺🍻🍼🎀🎁🎂🎃🎄🎅🎈🎉🎊🎋🎌🎍🎎🎏🎒🎓🎠🎡🎢🎣🎤🎥🎦🎧🎨🎩🎪🎫🎬🎭🎮🎯🎰🎱🎲🎳🎴🎵🎷🎸🎹🎺🎻🎽🎾🎿🏀🏁🏂🏃🏄🏆🏇🏈🏉🏊🐀🐁🐂🐃🐄🐅🐆🐇🐈🐉🐊🐋🐌🐍🐎🐏🐐🐑🐒🐓🐔🐕🐖🐗🐘🐙🐚🐛🐜🐝🐞🐟🐠🐡🐢🐣🐤🐥🐦🐧🐨🐩🐪🐫🐬🐭🐮🐯🐰🐱🐲🐳🐴🐵🐶🐷🐸🐹🐺🐻🐼🐽🐾👀👂👃👄👅👆👇👈👉👊👋👌👍👎👏👐👑👒👓👔👕👖👗👘👙👚👛👜👝👞👟👠👡👢👣👤👥👦👧👨👩👪👮👯👺👻👼👽👾👿💀💁💂💃💄💅";

char *argv0;

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

  size_t hlen;
  ssh_key pubkey;
  char *hexa;
  int i;

  char *hostname; 
  unsigned int port;
  ssh_options_get(session, SSH_OPTIONS_HOST, &hostname);
  ssh_options_get_port(session, &port);

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

      if (ssh_get_publickey_hash(pubkey, SSH_PUBLICKEY_HASH_MD5, &hostkeys[i], &hlen) != 0) {
        fprintf(stderr, "Error getting public key hash: %s\n", ssh_get_error(session));
        return -1;
      }

      hexa = ssh_get_hexa(hostkeys[i], hlen);
      printf("%s (%s)\n", hexa, hostkeytypes[i]);

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
  argv0 = argv[0];
  char *hostname = "localhost";
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

