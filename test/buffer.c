#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>
#include <setjmp.h>
#include <cmocka.h>

#include "../src/dynamic/buffer.h"

void core()
{
  buffer *b;
  char *s;

  b = buffer_new();
  assert_true(b);
  assert_int_equal(buffer_size(b), 0);
  assert_int_equal(buffer_capacity(b), 0);
  assert_int_equal(buffer_compact(b), 0);

  s = "some data";
  buffer_insert(b, 0, s, strlen(s) + 1);
  assert_int_equal(buffer_size(b), strlen(s) + 1);
  assert_int_equal(buffer_capacity(b), buffer_roundup(strlen(s) + 1));

  s = "random ";
  buffer_insert(b, 5, s, strlen(s));
  buffer_insert(b, buffer_size(b) - 1, ".", 1);
  assert_string_equal(buffer_data(b), "some random data.");
  buffer_free(b);

  s = "[string]";
  b = buffer_new();
  buffer_insert(b, 0, s, strlen(s) + 1);
  buffer_erase(b, 0, 1);
  buffer_erase(b, buffer_size(b) - 2, 1);
  s = buffer_deconstruct(b);
  assert_string_equal(s, "string");
  free(s);
}

extern int debug_out_of_memory;

void memory()
{
  buffer *b;
  int e;

  debug_out_of_memory = 1;
  b = buffer_new();
  assert_false(b);
  debug_out_of_memory = 0;

  b = buffer_new();
  assert_true(b);
  debug_out_of_memory = 1;
  e = buffer_insert(b, 0, "xxx", 3);
  assert_int_equal(e, -1);
  debug_out_of_memory = 0;

  e = buffer_insert(b, 0, "xxx", 3);
  assert_int_equal(e, 0);
  debug_out_of_memory = 1;
  e = buffer_compact(b);
  assert_int_equal(e, -1);
  debug_out_of_memory = 0;

  buffer_free(b);
}

int main()
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(core),
    cmocka_unit_test(memory)
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
