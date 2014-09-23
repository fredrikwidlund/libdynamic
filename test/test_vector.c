#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>
#include <setjmp.h>
#include <google/cmockery.h>

#include "buffer.h"
#include "vector.h"

void release(void *object)
{
  free(*(char **) object);
}

void *copy(void *data, size_t size)
{
  void *p;

  p = malloc(size);
  if (!p)
    return NULL;
  
  memcpy(p, data, size);
  return p;
}

void core()
{
  vector *v;
  char *s[] = {"a", "list", "of", "string", "pointers"}, *s2, **s3;
  int e;

  v = vector_new(sizeof(char *));
  assert_true(v);
  assert_true(vector_empty(v));
  assert_int_equal(vector_size(v), 0);
  assert_int_equal(vector_capacity(v), 0);
  
  vector_insert(v, 0, sizeof s / sizeof *s, s);
  assert_int_equal(vector_size(v), sizeof s / sizeof *s);
  
  vector_erase(v, 0, 1);
  assert_string_equal(*(char **) vector_front(v), "list"); 
  assert_string_equal(*(char **) vector_back(v), "pointers");

  vector_erase(v, vector_size(v) - 1, vector_size(v));
  assert_string_equal(*(char **) vector_back(v), "string");
  
  vector_pop_back(v);
  s2 = "something";
  vector_push_back(v, &s2);
  assert_string_equal(*(char **) vector_at(v, vector_size(v) - 1), s2);
  vector_free(v);
  
  v = vector_new(sizeof(char *));
  assert_true(v);
  vector_release(v, release);
  
  e = vector_reserve(v, 16);
  assert_int_equal(e, 0);
  assert_int_equal(vector_size(v), 0);
  assert_int_equal(vector_capacity(v), 16);

  s2 = copy(s[0], strlen(s[0]) + 1);
  e = vector_push_back(v, &s2);
  assert_int_equal(e, 0);

  s2 = copy(s[1], strlen(s[1]) + 1);
  e = vector_push_back(v, &s2);
  assert_int_equal(e, 0);

  s3 = vector_data(v);
  assert_string_equal(s3[0], "a");

  e = vector_shrink_to_fit(v);
  assert_int_equal(e, 0);
  assert_int_equal(vector_size(v), vector_capacity(v));

  s3 = vector_deconstruct(v);
  assert_string_equal(s3[0], "a");
  assert_string_equal(s3[1], "list");
  free(s3[0]);
  free(s3[1]);
  free(s3);
}

int main()
{
  const UnitTest tests[] = {
    unit_test(core)
  };

  return run_tests(tests);
}

