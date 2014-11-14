#include <stdio.h>
#include <stdlib.h>
#include <libssh/libssh.h>

int main() {
  printf("EMOTIVE SSH CLIENT...\n");

  size_t hlen;
  ssh_session session;
  ssh_key srv_pubkey;
  unsigned char *hash = NULL;
  char *hexa;
  char buf[10];

  session = ssh_new();
  if (session == NULL) return -1;

  ssh_options_set(session, SSH_OPTIONS_HOST, "localhost");

  if (ssh_connect(session) != SSH_OK) {
    fprintf(stderr, "Error connecting to localhost: %s\n", ssh_get_error(session));
    return -1;
  }

  /*hlen = ssh_get_pubkey_hash(session, &hash);*/
  if (ssh_get_publickey(session, &srv_pubkey) != SSH_OK) {
    fprintf(stderr, "Error getting public key: %s\n", ssh_get_error(session));
    return -1;
  }

  if (ssh_get_publickey_hash(srv_pubkey, SSH_PUBLICKEY_HASH_MD5, &hash, &hlen) != 0) {
    fprintf(stderr, "Error getting public key hash: %s\n", ssh_get_error(session));
    return -1;
  }

  hexa = ssh_get_hexa(hash, hlen);
  printf("%s\n", hexa);

  ssh_key_free(srv_pubkey);
  ssh_clean_pubkey_hash(&hash);
  ssh_free(session);

  return 0;
}
