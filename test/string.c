#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>

#include "dynamic/string.h"

void test_string(__attribute__((unused)) void **arg)
{
  char *cstr;
  string_t s;

  s = string("test");
  assert_false(string_empty(s));
  assert_true(string_end(s) == string_base(s) + string_size(s));
  (void) fwrite(string_base(s), string_size(s), 1, stdout);

  s = string_null();
  assert_true(string_empty(s));

  /* strdup breaks in valgrind with cmocka */
  cstr = malloc(5);
  memcpy(cstr, "test", 5);
  s = string(cstr);
  s = string_clear(s);
  assert_true(string_empty(s));
}

int main()
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_string)
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
