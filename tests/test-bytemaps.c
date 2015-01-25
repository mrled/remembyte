#include "../modules/minunit.h"

char *test_hex2buf() {
  return "Failed";
}

char *all_tests() {
  mu_suite_start();

  mu_run_test(test_hex2buf);

  return NULL;
}

RUN_TESTS(all_tests);