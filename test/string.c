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
  string *s, *s2;
  vector *v;
  char *cp;
  size_t n;
  int e;

  /* string_new() / string_data() */

  s = string_new("hello world");
  assert_string_equal(string_data(s), "hello world");

  /* string_clear() */

  string_clear(s);
  assert_string_equal(string_data(s), "");
  assert_int_equal(string_length(s), 0);
  assert_int_equal(string_capacity(s), 0);

  /* string_reserve() */

  e = string_reserve(s, 20);
  assert_true(e == 0);
  assert_int_equal(string_capacity(s), 31);

  /* string_shrink_to_fit() */

  e = string_shrink_to_fit(s);
  assert_true(e == 0);
  assert_int_equal(string_capacity(s), 0);
  assert_true(string_empty(s));

  /* string_insert() */

  e = string_insert(s, 42, "overrun");
  assert_true(e == -1);

  /* string_prepend() / string_append() */

  e = string_prepend(s, "hello");
  assert_true(e == 0);
  e = string_append(s, "world");
  assert_true(e == 0);
  e = string_insert(s, 5, " ");
  assert_true(e == 0);
  assert_string_equal(string_data(s), "hello world");

  /* string_erase() */

  e = string_erase(s, 5, 6);
  assert_true(e == 0);
  assert_string_equal(string_data(s), "hello");

  e = string_erase(s, string_length(s) + 1, 0);
  assert_true(e == -1);

  /* string_replace() */

  e = string_replace(s, 2, 2, "llo w");
  assert_true(e == 0);
  e = string_replace(s, string_length(s), 0, "rld");
  assert_true(e == 0);
  assert_string_equal(string_data(s), "hello world");

  e = string_replace(s, string_length(s) + 1, 0, "llo w");
  assert_true(e == -1);

  /* string_copy() */

  cp = malloc(6);
  assert_true(cp != NULL);
  e = string_copy(s, cp, 7, 42, &n);
  assert_true(e == -1);

  e = string_copy(s, cp, 5, 0, &n);
  assert_true(e == 0);
  assert_true(n == 5);

  cp[n] = 0;
  assert_string_equal(cp, "hello");
  free(cp);

  /* string_find() */

  e = string_append(s, ", world");
  assert_true(e == 0);
  n = string_find(s, "world", 0);
  assert_int_equal(n, 6);

  n = string_find(s, "world", 42);
  assert_int_equal(n, -1);
  n = string_find(s, "potato", 0);
  assert_int_equal(n, -1);

  /* string_substr() */

  s2 = string_substr(s, 42, 11);
  assert_true(s2 == NULL);

  s2 = string_substr(s, 0, 11);
  assert_true(s2);
  assert_string_equal(string_data(s2), "hello world");
  string_free(s);
  s = s2;
  string_free(s);

  /* string_compare() */

  s = string_new("test1");
  s2 = string_new("test2");
  assert_true(s);
  assert_true(s2);
  assert_true(string_compare(s, s2) < 0);
  assert_true(string_compare(s, s) == 0);
  assert_true(string_compare(s2, s) > 0);
  string_free(s);
  string_free(s2);

  /* string_split() */

  s = string_new("  a list, of  words ");
  v = string_split(s, " ,");
  string_free(s);

  assert_int_equal(vector_size(v), 4);
  assert_string_equal("words", string_data(*(string **) vector_at(v, 3)));
  vector_free(v);

  /* string_replace_all() */

  s = string_new("some random words in random order");
  string_replace_all(s, "random", "chosen");
  string_replace_all(s, "e", "");
  string_replace_all(s, "none", "other");
  assert_string_equal(string_data(s), "som chosn words in chosn ordr");

  cp = string_deconstruct(s);
  assert_string_equal(cp, "som chosn words in chosn ordr");
  free(cp);
}

void memory()
{
  extern int debug_out_of_memory;
  string *s, *s2;
  int e;

  debug_out_of_memory = 1;
  s = string_new("");
  assert_false(s);
  debug_out_of_memory = 0;

  s = string_new("");
  debug_out_of_memory = 1;
  e = string_append(s, "data");
  assert_true(e == -1);
  debug_out_of_memory = 0;
  string_free(s);

  s = string_new("");
  debug_out_of_memory = 1;
  s2 = string_substr(s, 0, 0);
  assert_false(s2);
  debug_out_of_memory = 0;
  string_free(s);

  s = string_new("aaaa");
  debug_out_of_memory = 1;
  e = string_replace_all(s, "a", "12345678");
  assert_true(e == -1);
  debug_out_of_memory = 0;
  string_free(s);
}

int main()
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(core),
    cmocka_unit_test(memory)
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
