#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "dynamic.h"

static void data_tests([[maybe_unused]] void **arg)
{
  data_t d1, d2;

  /* comparisons */
  d1 = data_null();
  assert_true(data_nullp(d1));
  assert_true(data_emptyp(d1));
  assert_true(data_equalp(data_null(), data_string("")));
  assert_false(data_equalp(data_string("b"), data_string("a")));
  assert_false(data_equalp(data_string("aa"), data_string("a")));

  /* offset */
  d1 = data_string("abcd");
  assert_int_equal(data_offset(d1, data_consume(d1, 2)), 2);
  assert_true(data_equalp(data_string("ab"), data_shift(data_consume(d1, 2), -2)));

  /* find */
  d1 = data_string("abcdef");
  d2 = data_find(d1, data_null());
  assert_true(data_emptyp(d2));
  assert_int_equal(data_offset(d1, d2), 0);
  d2 = data_find(d1, data_string("cd"));
  assert_int_equal(data_offset(d1, d2), 2);
  d2 = data_find(d1, data_string("gh"));
  assert_int_equal(data_offset(d1, d2), data_size(d1));
  assert_true(data_emptyp(d2));

  /* alloc, move, copy, insert, free */
  d1 = data_alloc(1024);
  assert_true(data_emptyp(d1));
  d1 = data_resize(d1, 4);
  (void) data_copy(d1, data_string("abcd"));
  (void) data_move(d1, data_range(d1, 2, 4));
  (void) data_insert(d1, 0, data_string("xx"));
  assert_true(data_equalp(d1, data_string("xxcd")));
  d1 = data_free(d1);
}

static void string_tests([[maybe_unused]] void **arg)
{
  string_t s1, s2;
  char storage[4];
  FILE *f;

  /* comparisons */
  s1 = string("http");
  assert_true(!string_emptyp(s1));
  assert_true(!string_nullp(s1));
  assert_int_equal(string_size(s1), 4);
  assert_true(string_equalp(s1, string("http")));

  /* lookup */
  s1 = string_find(string("abc cde fgh"), string("bla"));
  assert_true(string_emptyp(s1));

  /* string insert, append, prepend  erase */
  s1 = string_concat(string("http"), string("://"), string("a.b.c.d"));
  s1 = string_prepend(s1, string("<"));
  s1 = string_append(s1, string(">"));

  assert_memory_equal(string_base(s1), "<http://a.b.c.d>", string_size(s1));
  s1 = string_erase(s1, 1, 5);
  assert_memory_equal(string_base(s1), "<://a.b.c.d>", string_size(s1));
  string_free(s1);

  /* appends */
  s1 = string_null();
  for (int i = 1; i < 128; i ++)
    s1 = string_append(s1, string((char []) {i, 0}));
  assert_int_equal(string_size(s1), 127);
  string_free(s1);

  /* duplicating null will give an empty string */
  s1 = string_copy(string_null());
  assert_true(string_emptyp(s1));
  assert_true(!string_nullp(s1));
  string_free(s1);

  /* replace */
  s1 = string_replace(string("bla"), string(""), string("bleh"));
  assert_true(string_nullp(s1));
  string_free(s1);
  s1 = string_replace(string("bla"), string("bla"), string(""));
  assert_true(string_emptyp(s1));
  string_free(s1);
  s1 = string_replace(string("bla bla bleh bla bleh"), string("bla"), string("bleh"));
  assert_true(string_equalp(s1, string("bleh bleh bleh bleh bleh")));
  s2 = string_replace(s1, string("bleh"), string("."));
  string_free(s1);
  assert_true(string_equalp(s2, string(". . . . .")));
  string_free(s2);

  /* unicode */
  s1 = string("abc\u00e5\u0250\u00e4\u03B6\ufffd\u2103\u25A2\u2780\U0001F600");
  assert_true(string_valid(s1));
  assert_int_equal(string_length(s1), 12);
  f = fopen("/dev/null", "a");
  assert_int_equal(string_fwrite(s1, f), 1);
  fclose(f);
  assert_true(string_valid(string_character(data(storage, sizeof storage), 'x')));
  assert_true(string_valid(string_character(data(storage, sizeof storage), 0x03b6)));
  assert_true(string_valid(string_character(data(storage, sizeof storage), 0x2780)));
  assert_true(string_valid(string_character(data(storage, sizeof storage), 0x0001f700)));
  assert_int_equal(string_codepoint(string("@")), 0x40);
  assert_int_equal(string_codepoint(string("\xc2")), 0xfffd);
  assert_int_equal(string_codepoint(string("\xc2\xc2")), 0xfffd);
  assert_int_equal(string_codepoint(string("\U0001f700")), 0x0001f700);
  assert_true(string_nullp(string_first(string("\xc2\xc2"))));
  assert_false(string_valid(string("\xc2\xc2")));
}

static void vector_tests([[maybe_unused]] void **arg)
{
  vector_t v;
  uint8_t i;
  uint16_t s;

  v = vector(uint16_t, 1, 2, 3, 5, 8, 13);
  assert_false(vector_emptyp(v));
  assert_int_equal(vector_at(v, uint16_t, 0), 1);
  assert_int_equal(vector_back(v, uint16_t), 13);
  assert_int_equal(*(uint16_t *) vector_base(v), 1);
  assert_int_equal(vector_size(v), 12);
  assert_int_equal(vector_length(v, uint16_t), 6);

  v = vector_copy(v);
  i = 0;
  vector_foreach(v, uint16_t, s)
    assert_int_equal(s, vector_at(vector(uint16_t, 1, 2, 3, 5, 8, 13), uint16_t, i++));

  v = vector_erase(v, uint16_t, 1, 5);
  assert_true(vector_equalp(v, vector(uint16_t, 1, 13)));
  v = vector_free(v);

  v = vector();
  assert_true(vector_base(v) == NULL);
  for (i = 0; i < 128; i ++)
    v = vector_push_back(v, uint8_t, i);
  assert_int_equal(vector_length(v, uint8_t), 128);
  for (i = 0; i < 128; i ++)
    assert_int_equal(vector_at(v, uint8_t, i), i);
  for (i = 0; i < 128; i ++)
    v = vector_pop_back(v, uint8_t);
  assert_true(vector_emptyp(v));
  v = vector_free(v);
}

/* LIST */

static bool int_odd(list_iter_t *a)
{
  return *(int *)a % 2 == 1;
}

static void list_tests([[maybe_unused]] void **arg)
{
  list_t l, m;
  int i, e;

  /* int array */
  l = list(int, 10, 11, 12);
  l = list_push_front(l, int, 5, 6, 7, 8, 9);
  l = list_push_front(l, int, 1, 2, 3, 4);
  l = list_push_back(l, int, 13);
  l = list_push_back(l, int, 14);
  i = 1;
  list_foreach(l, int, e)
    assert_int_equal(i++, e);

  l = list_pop_back(l);
  l = list_pop_front(l);
  assert_int_equal(list_length(l), 12);
  l = list_free(l);
  assert_true(list_front(l) == NULL);
  assert_true(list_back(l) == NULL);
  l = list_push_back(l, int, 1);
  l = list_erase(l, list_front(l));
  assert_true(list_emptyp(l));

  l = list(int, 1, 2, 3);
  m = list(int, 4, 5, 6);
  m = list_splice(m, NULL, &l, list_back(l));
  m = list_splice(m, NULL, &l, list_back(l));
  m = list_splice(m, NULL, &l, list_back(l));
  assert_int_equal(list_length(l), 0);
  assert_int_equal(list_length(m), 6);
  m = list_remove(m, int, 1);
  m = list_remove(m, int, 6);
  m = list_remove_if(m, int_odd);
  /* list should be 2, 4 */
  assert_int_equal(list_length(m), 2);
  assert_int_equal(*(int *) list_front(m), 2);
  assert_int_equal(*(int *) list_back(m), 4);
  list_free(l);
  list_free(m);
}

static void buffer_tests([[maybe_unused]] void **arg)
{
  buffer_t b;

  /* basic operations */
  b = buffer();
  assert_true(buffer_emptyp(b));
  assert_true(data_nullp(buffer_data(b)));
  assert_true(data_nullp(buffer_memory(b)));

  /* inserts */
  b = buffer_insert(b, 0, string("test"));
  assert_true(string_equalp(buffer_data(b), string("test")));
  b = buffer_prepend(b, string("xx"));
  assert_true(string_equalp(buffer_data(b), string("xxtest")));
  b = buffer_append(b, string("zz"));
  assert_true(string_equalp(buffer_data(b), string("xxtestzz")));
  b = buffer_insert(b, 4, string("yy"));
  assert_true(string_equalp(buffer_data(b), string("xxteyystzz")));
  b = buffer_reserve(b, 1024);
  assert_true(string_equalp(buffer_data(b), string("xxteyystzz")));
  b = buffer_insert(b, 0, string("pre1"));
  b = buffer_insert(b, 0, string("pre2"));
  assert_true(string_equalp(buffer_data(b), string("pre2pre1xxteyystzz")));

  /* erase */
  b = buffer_erase(b, 0, 8);
  assert_true(string_equalp(buffer_data(b), string("xxteyystzz")));
  b = buffer_erase(b, 2, 4);
  assert_true(string_equalp(buffer_data(b), string("xxyystzz")));
  b = buffer_erase(b, 4, 8);
  assert_true(string_equalp(buffer_data(b), string("xxyy")));
  b = buffer_erase(b, 0, 4);
  assert_true(string_equalp(buffer_data(b), string("")));
  assert_true(data_base(buffer_data(b)) == data_base(buffer_memory(b)));
  b = buffer_free(b);
}

static void pool_tests([[maybe_unused]] void **arg)
{
  pool_t p;
  void *m;

  p = pool();
  p = pool_alloc(p, &m, 32);
  p = pool_return(p, m);
  p = pool_alloc(p, &m, 32);
  p = pool_return(p, m);
  p = pool_free(p);
}

int main()
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(data_tests),
    cmocka_unit_test(string_tests),
    cmocka_unit_test(vector_tests),
    cmocka_unit_test(list_tests),
    cmocka_unit_test(buffer_tests),
    cmocka_unit_test(pool_tests)
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
