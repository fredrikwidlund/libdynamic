

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <sys/time.h>
#include <setjmp.h>
#include <cmocka.h>

#include "../src/map_str.h"

void release_value(char *key, void *value)
{
  free(key);
  (void) value;
}

char *test_strdup(char *key)
{
  return strcpy((char *) malloc(strlen(key) + 1), key);
}

void core()
{
  map_str *m;
  int value;
  
  /* allocate map */
  m = map_str_new(sizeof value);
  assert_true(m != NULL);
  assert_int_equal(map_str_size(m), 0);
  assert_int_equal(map_str_bucket_count(m), 0);

  /* clear empty map */
  map_str_clear(m);
  assert_int_equal(map_str_size(m), 0);
  
  /* insert object */
  value = 1000;
  map_str_insert(m, "foo", &value);
  assert_int_equal(map_str_size(m), 1);
  assert_int_equal(map_str_bucket_count(m), 2);
  
  /* insert duplicate */
  map_str_insert(m, "foo", &value);
  assert_int_equal(map_str_size(m), 1);
  
  /* insert collision (will collide with 'foo') */
  map_str_insert(m, "}xopjr{zsjredlz}miflwanoygrdq", &value);
  assert_int_equal(map_str_size(m), 2);
  
  /* lookup collided object */
  assert_int_equal(*(int *) map_str_at(m, "}xopjr{zsjredlz}miflwanoygrdq"), 1000);
  
  /* lookup non-existing object */
  assert_true(map_str_find(m, "bar") == map_str_end(m));
  assert_true(map_str_at(m, "bar") == NULL);
  
  /* erase non-existing object */
  map_str_erase(m, "bar");
  assert_int_equal(map_str_size(m), 2);

  /* erase object  */
  map_str_erase(m, "foo");
  map_str_erase(m, "}xopjr{zsjredlz}miflwanoygrdq");
  assert_int_equal(map_str_size(m), 0);
  
  /* insert on deleted position  */
  map_str_rehash(m, 4);
  map_str_insert(m, "foo", &value);
  map_str_erase(m, "foo");
  map_str_insert(m, "foo", &value);
  assert_int_equal(map_str_size(m), 1);
  assert_int_equal(map_str_bucket_count(m), 4);
  
  /* allocate map with allocated components */
  map_str_free(m);

  m = map_str_new(sizeof value);
  map_str_release(m, release_value);
  assert_true(m != NULL);
  assert_int_equal(map_str_size(m), 0);

  /* insert objects */
  value = 42;
  map_str_insert(m, test_strdup("foo"), &value);
  value = 4711;
  map_str_insert(m, test_strdup("bar"), &value);

  /* insert collision (will collide with 'foo') */
  map_str_insert(m, test_strdup("}xopjr{zsjredlz}miflwanoygrdq"), &value);

  /* insert duplicate */
  map_str_insert(m, test_strdup("bar"), &value);
  assert_int_equal(map_str_size(m), 3);

  /* lookup object */
  assert_true(*(int *) map_str_at(m, "foo") == 42);
  
  /* rehash to zero */
  map_str_rehash(m, 0);
  
  /* erase object with allocated components */
  map_str_erase(m, "foo");
  assert_int_equal(map_str_size(m), 2);
  
  map_str_free(m);
}

int main()
{
  const UnitTest tests[] = {
    unit_test(core)
  };

  return run_tests(tests);
}
