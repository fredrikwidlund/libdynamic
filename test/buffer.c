#include <stdlib.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>

#include "dynamic/buffer.h"

static data_t string(const char *cstr)
{
  return (data_t) {(void *) cstr, strlen(cstr)};
}

void test_buffer(__attribute__((unused)) void **arg)
{
  buffer_t b;

  /* rounding */
  b = buffer();
  buffer_reserve(&b, 1);
  assert_int_equal(buffer_capacity(b), 1);
  assert_int_equal(buffer_roundup(0), 0);
  assert_int_equal(buffer_roundup(1), 1);
  assert_int_equal(buffer_roundup(2), 2);
  assert_int_equal(buffer_roundup(3), 4);

  /* basic */
  buffer_prepend(&b, string("mid"));
  assert_int_equal(buffer_size(b), 3);
  assert_int_equal(buffer_capacity(b), 4);
  buffer_insert(&b, 0, string("start"));
  assert_int_equal(buffer_size(b), 8);
  assert_int_equal(buffer_capacity(b), 8);
  buffer_append(&b, string("end"));
  assert_int_equal(buffer_size(b), 11);
  assert_int_equal(buffer_capacity(b), 16);
  assert_memory_equal(buffer_base(b), "startmidend", 11);
  buffer_clear(&b);
  assert_int_equal(buffer_size(b), 0);
  assert_int_equal(buffer_capacity(b), 0);

  /* align for prepend and append */
  buffer_insert(&b, 0, string("5"));
  buffer_insert(&b, 0, string("4"));
  buffer_insert(&b, 0, string("3"));
  buffer_insert(&b, 0, string("2"));
  buffer_insert(&b, 0, string("1"));
  assert_int_equal(buffer_size(b), 5);
  assert_int_equal(buffer_capacity(b), 8);
  buffer_insert(&b, buffer_size(b), string("a"));
  buffer_insert(&b, buffer_size(b), string("b"));
  buffer_insert(&b, buffer_size(b), string("c"));
  buffer_insert(&b, 5, string("|"));
  assert_memory_equal(buffer_base(b), "12345|abc", 9);
  buffer_clear(&b);

  /* erase */
  buffer_insert(&b, 0, string("12345"));
  buffer_consume(&b, 1);
  assert_memory_equal(buffer_base(b), "2345", 4);
  buffer_erase(&b, 1, 1);
  assert_memory_equal(buffer_base(b), "245", 3);
  buffer_erase(&b, 2, 1);
  assert_memory_equal(buffer_base(b), "24", 2);
  buffer_consume(&b, 2);
  assert_int_equal(buffer_size(b), 0);
  assert_true(buffer_data(b).iov_base == buffer_base(b));
  assert_true(buffer_base(b) == buffer_end(b));
  assert_true(buffer_base(b) == buffer_memory(b));

  /* resize */
  buffer_resize(&b, 1024);
  assert_int_equal(buffer_size(b), 1024);
  assert_int_equal(buffer_capacity(b), 1024);
  buffer_erase(&b, 0, 1);
  buffer_resize(&b, 1024);
  assert_int_equal(buffer_size(b), 1024);
  assert_int_equal(buffer_capacity(b), 1024);
  buffer_resize(&b, 1);
  assert_int_equal(buffer_size(b), 1);
  buffer_erase(&b, 0, 1);
  buffer_resize(&b, 0);
  assert_int_equal(buffer_size(b), 0);
  assert_true(buffer_empty(b));
  buffer_clear(&b);
}

int main()
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_buffer)
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
