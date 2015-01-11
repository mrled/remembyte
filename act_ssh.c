#include "act_ssh.h"

ssh_hostkeys ssh_hostkeys_new() {
  ssh_hostkeys hk;
  hk.count = HOSTKEY_COUNT;
  hk.keytypes[0] = "ecdsa-sha2-nistp256";
  hk.keytypes[1] = "ssh-dss";
  hk.keytypes[2] = "ssh-rsa";
  hk.keylengths[hk.count] = 0;
  return hk;
}

/**
 * Get banners and version information for an SSH server
 * 
 * @param session an ssh_session pointer (from libssh), in a disconnected state.
 * @param banners pointer to an ssh_banners struct (defined in the header).
 * @return -1 for failures, 0 for successes
 */
int get_banners(ssh_session session, ssh_banners *banners) {
  if (ssh_connect(session) != SSH_OK) {
    char *hostname; 
    unsigned int port;
    ssh_options_get(session, SSH_OPTIONS_HOST, &hostname);
    ssh_options_get_port(session, &port);
    fprintf(stderr, "Error connecting to %s:%u - %s\n",
      hostname, port, ssh_get_error(session));
    return -1;
  }

  /* TODO: why doesn't ssh_get_issue_banner work? 
   * I can set a banner that openssh will display but this won't.
   */
  banners->issue_banner = ssh_get_issue_banner(session);
  banners->server_banner = ssh_get_serverbanner(session);

  /* TODO: parse OpenSSH version so it returns the real version.
   * I have no idea what this value is returning??
   */
  banners->openssh_version = ssh_get_openssh_version(session);

  /* This does not actually initiate a disconnect.*/
  banners->disconnect_message = ssh_get_disconnect_message(session);

  ssh_disconnect(session);
  return 0;
}

bool print_banners(ssh_banners *banners) {
  if (banners->issue_banner) {
    printf("Issue banner:\n%s\n", banners->issue_banner);
  }
  else {
    printf("No issue banner.\n");
  }

  if (banners->server_banner && strlen(banners->server_banner) >0) {
    printf("Server banner: %s.\n", banners->server_banner);
  }
  else {
    printf("No server banner.\n");
  }

  if (banners->openssh_version) {
    printf("OpenSSH version: %i.\n", banners->openssh_version);
  }
  else {
    printf("OpenSSH version: unavailable (or server is not OpenSSH).\n");
  }

  if (banners->disconnect_message) {
    printf("Disconnect message: %s.\n", banners->disconnect_message);
  }
  else {
    printf("No disconnect message.\n");
  }

  return true;
}

/**
 * Get host keys for an SSH server
 * 
 * @param session an ssh_session pointer (from libssh), in a disconnected state.
 * @param hostkeys a pointer to an ssh_hostkeys struct (defined in the header).
 * @return -1 for failures, 0 for successes
 */
int get_hostkey_fingerprint(ssh_session session, ssh_hostkeys *hostkeys) {

  int kctr;
  ssh_key pubkey = NULL;

  size_t hkhash_buf_len;
  unsigned char *hkhash_buf;

  for (kctr=0; kctr < hostkeys->count; kctr++) {
    if (ssh_options_set(session, SSH_OPTIONS_HOSTKEYS, hostkeys->keytypes[kctr]) != 0) {
      fprintf(stderr, "Error setting SSH option for host key '%s': %s\n", 
        hostkeys->keytypes[kctr], ssh_get_error(session));
      return -1;
    }

    if (ssh_connect(session) != SSH_OK) {
      // The server does not support the host key type.
      hostkeys->keyvalues[kctr] = (unsigned char*)"";
      hostkeys->keylengths[kctr] = 0;
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
      if (ssh_get_publickey_hash(pubkey, SSH_PUBLICKEY_HASH_MD5, &hkhash_buf, &hkhash_buf_len) != 0) {
        fprintf(stderr, "Error getting public key hash: %s\n", ssh_get_error(session));
        return -1;
      }
      hostkeys->keyvalues[kctr] = hkhash_buf;
      hostkeys->keylengths[kctr] = hkhash_buf_len;
    }
    ssh_disconnect(session);
  }

  if (pubkey != NULL) {
    ssh_key_free(pubkey);
  }

  return 0;
}

/*
bool print_hostkey_fingerprint(ssh_hostkeys *hostkeys, mapping_t mapping) {
  int kctr;
  char *display;

  for (kctr=0; kctr< hostkeys->count; kctr++) {
    if (hostkeys->keylengths[kctr]) {
      display = get_display_hash(hostkeys->keyvalues[kctr], hostkeys->keylengths[kctr], mapping);
      if (!display) {
        fprintf(stderr, "Error getting mapped buffer.\n");
        return false;
      }
      printf("%s (%s)\n", display, hostkeys->keytypes[kctr]);
    }
    else {
      printf("No key of type %s.\n", hostkeys->keytypes[kctr]);
    }
  }

  free(display);
  return true;
}
 */

