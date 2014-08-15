#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>
#include <setjmp.h>
#include <sys/resource.h>
#include <google/cmockery.h>

#include <vector.h>

void release(void *object)
{
  free(*(char **) object);
}

void core()
{
  vector *v;
  void *data;
  char *object, *strings[] = {"a", "vector", "of", "strings"};

  v = vector_new(sizeof(char *));
  assert_true(v != NULL);
  
  vector_reserve(v, 0);
  vector_reserve(v, 16);
  vector_insert(v, 0, sizeof strings / sizeof *strings, strings);
  assert_int_equal(vector_size(v), 4);
  assert_int_equal(vector_capacity(v), 16);
  
  vector_shrink_to_fit(v);
  assert_int_equal(vector_capacity(v), 4);
  
  vector_resize(v, 5);
  *(char **) vector_at(v, 4) = "added";
  assert_int_equal(vector_size(v), 5);

  vector_erase(v, 3, 4);
  assert_int_equal(vector_size(v), 4);
  vector_push_back(v, &strings[3]);
  assert_int_equal(vector_size(v), 5);
  
  assert_string_equal(*(char **) vector_front(v), "a");
  assert_string_equal(*(char **) vector_back(v), "strings"); 
  
  vector_pop_back(v);
  vector_pop_back(v);
  vector_push_back(v, &strings[3]);
  assert_memory_equal(vector_data(v), strings, sizeof strings / sizeof *strings);

  vector_clear(v);
  assert_int_equal(vector_size(v), 0);
  assert_int_equal(vector_capacity(v), 0);

  vector_release(v, release);
  object = (char *) malloc(strlen(strings[0]) + 1);
  strcpy(object, strings[0]);
  vector_push_back(v, &object);
  object = (char *) malloc(strlen(strings[1]) + 1);
  strcpy(object, strings[1]);
  vector_push_back(v, &object);
  assert_int_equal(vector_size(v), 2);
  assert_int_equal(vector_capacity(v), 2);
  assert_true(!vector_empty(v));
  
  vector_free(v);

  v = vector_new(1);
  vector_insert(v, 0, 5, "test");
  data = vector_deconstruct(v);
  assert_string_equal((char *) data, "test");
  free(data);
}

int main()
{
  const UnitTest tests[] = {
    unit_test(core)
  };

  return run_tests(tests);
}
