#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>

#include "dynamic/data.h"

void test_data(__attribute__((unused)) void **arg)
{
  data_t d;

  d = data_string("test");
  assert_false(data_empty(d));
  assert_true(data_end(d) == data_base(d) + data_size(d));
  (void) fwrite(data_base(d), data_size(d), 1, stdout);

  d = data_null();
  assert_true(data_empty(d));

  d = data_copy(data_string("test"));
  assert_int_equal(data_size(d), 4);
  data_clear(&d);
  assert_true(data_empty(d));

  d = data_copy_terminate(data("test", 4));
  printf("%s\n", data_base(d));
  data_clear(&d);

  assert_false(data_equal(data_string("a"), data_string("aa")));
  assert_true(data_equal(data_string("a"), data_string("a")));
  assert_false(data_equal(data_string("a"), data_string("b")));

  d = data_alloc(123);
  data_realloc(&d, 1);
  assert_int_equal(data_size(d), 1);
  data_clear(&d);
}

int main()
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_data)
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
