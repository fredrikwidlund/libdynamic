#include <stdlib.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>

#include "dynamic/vector.h"

void test_vector(__attribute__((unused)) void **arg)
{
  vector_t v;

  assert_int_equal(vector_roundup(0), 0);
  assert_int_equal(vector_roundup(1), 1);
  assert_int_equal(vector_roundup(2), 2);
  assert_int_equal(vector_roundup(3), 4);

  v = vector();
  assert_true(vector_base(v) == NULL);
  assert_true(vector_empty(v));

  v = vector();
  vector_resize(&v, 4);
  assert_int_equal(vector_size(v), 4);
  vector_resize(&v, 3);
  assert_int_equal(vector_size(v), 3);
  vector_clear(&v);
  assert_true(vector_empty(v));

  vector_push(&v, int, 1, 2, 3);
  assert_int_equal(vector_length(v, int), 3);
  vector_pop(&v, int);
  assert_int_equal(vector_length(v, int), 2);
  vector_clear(&v);
}

int main()
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_vector)
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
