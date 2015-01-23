#include "act_ssh.h"

// TODO: need a corresponding _free()
// TODO: names in this file shouldn't look like libssh function names
// TODO: iterate over libssh for keytypes


/* In libssh.h: 

enum ssh_keytypes_e{
  SSH_KEYTYPE_UNKNOWN=0,
  SSH_KEYTYPE_DSS=1,
  SSH_KEYTYPE_RSA,
  SSH_KEYTYPE_RSA1,
  SSH_KEYTYPE_ECDSA,
  SSH_KEYTYPE_ED25519
};

 * TODO: My understanding of SSH key names in libssh is not yet complete
 * Like, search the codebase for ecdsa-sha2-nistp256 for example
 * This doesn't seem to expose keys like that
 * Also it seems if I pass that key type to ssh_options_set as in:

    ssh_options_set(session, SSH_OPTIONS_HOSTKEYS, "ecdsa-sha2-nistp256");

 * ... it works? But it doesn't get returned for anything from 
 * ssh_key_type_to_char() ?
  */
ssh_hostkeys *ssh_hostkeys_new() {
  int ix;
  char *hktype_name;

  ssh_hostkeys *hk=NULL;
  hk = malloc(sizeof(ssh_hostkeys));
  check_mem(hk);
  
  hk->count = SSH_KEYTYPE_ECDSA;

  hk->keytypes = malloc(sizeof(int) * hk->count);
  hk->keyvalues = malloc(sizeof(unsigned char*) * hk->count);
  hk->keylengths = malloc(sizeof(size_t) * hk->count);

  for (ix=0; ix <= hk->count; ix++) {
    hktype_name = (char *) ssh_key_type_to_char(ix);
    log_debug("Looking for library support for host keys of type %s "
      "(enum value %i)", hktype_name, ix);

    hk->keytypes[ix] = ix;
    hk->keylengths[ix] = 0;
  }

  return hk;

error:
  free(hk);
  return NULL;
}

void ssh_hostkeys_free(ssh_hostkeys *hk) {
  free(hk);
}

ssh_banners *ssh_banners_new() {
  ssh_banners *ban=NULL;
  ban = malloc(sizeof(ssh_banners));
  check_mem(ban);

  ban->issue_banner = NULL;
  ban->server_banner = NULL;
  ban->openssh_version = 0;
  ban->disconnect_message = NULL;

error:
  free(ban);
  return NULL;
}

/* Take an integer from ssh_get_openssh_version() and print the version it 
 * represents in dot notation (e.g. 1.2.3)
 * 
 * This is useful because the verison is bitwise-shifted... in libssh.h, a 
 * macro is defined to do the shifting: 
 *
 *     #define SSH_VERSION_INT(a, b, c) ((a) << 16 | (b) << 8 | (c))
 *
 * But since I don't see one for getting the dot notation from the int, I wrote
 * one myself.
 */
char *osshv2a(int osshv) {
  int major, minor, patch, major_mask, minor_mask, patch_mask;
  size_t vstr_len;
  char *vstr;

  major_mask = 255 << 16;
  minor_mask = 255 << 8;
  patch_mask = 255;

  major = (osshv & major_mask) >> 16;
  minor = (osshv & minor_mask) >> 8;
  patch = osshv & patch_mask;

  // TODO: can't use snprintf in real code
  vstr_len = snprintf(NULL, 0, "%i.%i.%i", major, minor, patch);
  vstr = malloc((sizeof(char) * vstr_len) +1);
  snprintf(vstr, vstr_len +1,  "%i.%i.%i", major, minor, patch);
  return vstr;
}

/**
 * Get banners and version information for an SSH server
 * 
 * @param session an ssh_session pointer (from libssh), in a disconnected state.
 * @param banners pointer to an ssh_banners struct (defined in the header).
 * @return -1 for failures, 0 for successes
 */
int get_banners(ssh_session session, ssh_banners *banners) {
  char *hostname = NULL;
  unsigned int port = 0;

  ssh_options_get(session, SSH_OPTIONS_HOST, &hostname);
  ssh_options_get_port(session, &port);

  check(session != SSH_OK, "Error connecting to %s:%u - '%s'", 
    hostname, port, ssh_get_error(session));

  // TODO: why doesn't ssh_get_issue_banner work? 
  // I can set a banner that openssh will display but this won't.
  banners->issue_banner = ssh_get_issue_banner(session);
  banners->server_banner = ssh_get_serverbanner(session);

  banners->openssh_version_int = ssh_get_openssh_version(session);
  banners->openssh_version = osshv2a(banners->openssh_version_int);

  /* This does not actually initiate a disconnect.*/
  banners->disconnect_message = ssh_get_disconnect_message(session);

  ssh_disconnect(session);

  // TODO: Free memory here duh

  return 0;

error:
  return -1;
}

/*
 * Get host keys for an SSH server
 * 
 * @param session an ssh_session pointer (from libssh), in a disconnected state.
 * @param hostkeys a pointer to an ssh_hostkeys struct (defined in the header).
 * @return -1 for failures, 0 for successes
 */
int get_hostkey_fingerprint(ssh_session session, ssh_hostkeys *hostkeys) {
  int kctr, rc, ix;
  ssh_key pubkey = NULL;
  size_t hkhash_buf_len;
  unsigned char *hkhash_buf;
  char *hktype_name;

  for (kctr=0; kctr < hostkeys->count; kctr++) {

    hktype_name = (char *) ssh_key_type_to_char(hostkeys->keytypes[kctr]);
    hkhash_buf = (unsigned char*)"";
    hkhash_buf_len = 0;

    rc = ssh_options_set(session, SSH_OPTIONS_HOSTKEYS, hktype_name);
    if (rc != 0) {
      log_debug("libssh apparently doesn't support the %s algorithm?",
        hktype_name);
      continue;
    }
    //check(rc == 0, "Error (%i) setting SSH option for host key '%s': %s\n", 
    //  rc, hktype_name, ssh_get_error(session));

    rc = ssh_connect(session);
    if (rc != SSH_OK) {
      log_debug("Server does not support type %s", hktype_name);
    }

    else {
      log_debug("Found server key of type %s", hktype_name);

      rc = ssh_get_publickey(session, &pubkey);
      check(rc == SSH_OK, "Error getting public key: %s\n", 
        ssh_get_error(session));

      /* The third ("hash") argument to ssh_get_publickey_hash comes back as a 
       * character buffer of hex values. However, it's padded with zeroes. 

         (lldb) frame variable hash
         (unsigned char *) hostkey = 0x000000000119c860 "\x81\x0f\x13)\xab\xf2\xb4g\xd0\x13\x89w\x96]o\x01XXXXXXXX1"
         (lldb) frame variable ssh_get_hexa(hash, hash_len) // this is lldb pseudocode, of course
         (char *) hexa = 0x000000000119db70 "81:0f:13:29:ab:f2:b4:67:d0:13:89:77:96:5d:6f:01"
       */
      rc = ssh_get_publickey_hash(pubkey, SSH_PUBLICKEY_HASH_MD5, &hkhash_buf, 
        &hkhash_buf_len);
      check(rc == 0, "Error getting public key hash: %s\n", 
        ssh_get_error(session));
    }

    hostkeys->keyvalues[kctr] = hkhash_buf;
    hostkeys->keylengths[kctr] = hkhash_buf_len;

    ssh_silent_disconnect(session);
  }

  if (pubkey) ssh_key_free(pubkey);
  return 0;

error:
  if (pubkey) ssh_key_free(pubkey);
  return -1;
}
