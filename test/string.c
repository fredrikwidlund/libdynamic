#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>
#include <setjmp.h>
#include <cmocka.h>

#include "../src/dynamic/buffer.h"
#include "../src/dynamic/vector.h"
#include "../src/dynamic/string.h"

void core()
{
  string s, s2;
  vector v;
  ssize_t i;

  string_construct(&s);
  assert_int_equal(string_length(&s), 0);
  assert_int_equal(string_capacity(&s), 0);
  assert_true(string_empty(&s));

  string_reserve(&s, 1023);
  assert_int_equal(string_capacity(&s), 1023);
  string_shrink_to_fit(&s);
  assert_int_equal(string_capacity(&s), 0);

  string_insert(&s, 0, "insert");
  string_prepend(&s, "prepend");
  string_append(&s, "append");
  assert_string_equal(string_data(&s), "prependinsertappend");

  i = string_find(&s, "insert", 0);
  assert_true(i >= 0);
  string_erase(&s, i, 6);
  assert_string_equal(string_data(&s), "prependappend");
  string_replace_all(&s, "pend", "-");

  string_construct(&s2);
  string_append(&s2, "pre-ap-");
  assert_true(string_compare(&s, &s2) == 0);
  string_destruct(&s2);
  string_clear(&s);
  assert_true(string_empty(&s));
  string_destruct(&s);

  string_construct(&s);
  string_append(&s, " some space   delimited string ");
  string_split(&s, " ", &v);
  assert_int_equal(vector_size(&v), 4);
  assert_string_equal(string_data(vector_at(&v, 0)), "some");
  assert_string_equal(string_data(vector_at(&v, 1)), "space");
  assert_string_equal(string_data(vector_at(&v, 2)), "delimited");
  assert_string_equal(string_data(vector_at(&v, 3)), "string");
  vector_destruct(&v);
  string_destruct(&s);
}

int main()
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(core)
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
