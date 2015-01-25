#include "../modules/minunit.h"
#include "../modules/bytemaps.h"

char *test_hex2buf() {
  size_t buf_sz;
  int cmp;
  unsigned char *buf;
  char *compare = "drugs are good";
  char *hs = "64727567732061726520676F6F6400";

  buf_sz = hex2buf(hs, &buf);
  check(buf_sz == strlen(compare)+1, "");

  cmp = strncmp( (const char*)buf, compare, buf_sz);
  check(cmp==0, "");

  return NULL;
error:
  return "hex2buf() returned bad value";
}

char *all_tests() {
  mu_suite_start();

  mu_run_test(test_hex2buf);

  return NULL;
}

RUN_TESTS(all_tests);