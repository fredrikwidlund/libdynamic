#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>
#include <setjmp.h>
#include <cmocka.h>

#include "../src/dynamic/buffer.h"

extern int debug_out_of_memory;
extern int debug_abort;

void core()
{
  buffer b;

  buffer_construct(&b);
  assert_int_equal(buffer_size(&b), 0);
  assert_int_equal(buffer_capacity(&b), 0);

  buffer_reserve(&b, 0);
  buffer_reserve(&b, 1024);
  assert_int_equal(buffer_capacity(&b), 1024);

  buffer_compact(&b);
  buffer_compact(&b);
  assert_int_equal(buffer_capacity(&b), 0);

  buffer_insert(&b, 0, "last", 4);
  buffer_insert(&b, 0, "first", 5);
  buffer_insert(&b, buffer_size(&b), "", 1);
  assert_string_equal(buffer_data(&b), "firstlast");

  buffer_erase(&b, 5, 4);
  assert_string_equal(buffer_data(&b), "first");

  buffer_insert_fill(&b, 0, 5, "x", 1);
  assert_string_equal(buffer_data(&b), "xxxxxfirst");
  buffer_insert_fill(&b, buffer_size(&b), 5, "x", 1);
  assert_string_equal(buffer_data(&b), "xxxxxfirst");

  buffer_destruct(&b);
}

void alloc()
{
  buffer b;

  buffer_construct(&b);
  debug_out_of_memory = 1;
  debug_abort = 1;
  expect_assert_failure(buffer_reserve(&b, 100));
  debug_abort = 0;
  debug_out_of_memory = 0;
  buffer_destruct(&b);
}

int main()
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(core),
    cmocka_unit_test(alloc),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
