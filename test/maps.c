#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <sys/time.h>
#include <setjmp.h>
#include <cmocka.h>

#include "../src/dynamic/maps.h"

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
  maps *m;
  int value;

  /* allocate map */
  m = maps_new(sizeof value);
  assert_true(m != NULL);
  assert_int_equal(maps_size(m), 0);
  assert_int_equal(maps_bucket_count(m), 0);

  /* clear empty map */
  maps_clear(m);
  assert_int_equal(maps_size(m), 0);

  /* insert object */
  value = 1000;
  maps_insert(m, "foo", &value);
  assert_int_equal(maps_size(m), 1);
  assert_int_equal(maps_bucket_count(m), 2);

  /* insert duplicate */
  maps_insert(m, "foo", &value);
  assert_int_equal(maps_size(m), 1);

  /* insert collision (will collide with 'foo') */
  maps_insert(m, "}xopjr{zsjredlz}miflwanoygrdq", &value);
  assert_int_equal(maps_size(m), 2);

  /* lookup collided object */
  assert_int_equal(*(int *) maps_at(m, "}xopjr{zsjredlz}miflwanoygrdq"), 1000);

  /* lookup non-existing object */
  assert_true(maps_find(m, "bar") == maps_end(m));
  assert_true(maps_at(m, "bar") == NULL);

  /* erase non-existing object */
  maps_erase(m, "bar");
  assert_int_equal(maps_size(m), 2);

  /* erase object  */
  maps_erase(m, "foo");
  maps_erase(m, "}xopjr{zsjredlz}miflwanoygrdq");
  assert_int_equal(maps_size(m), 0);

  /* insert on deleted position  */
  maps_rehash(m, 4);
  maps_insert(m, "foo", &value);
  maps_erase(m, "foo");
  maps_insert(m, "foo", &value);
  assert_int_equal(maps_size(m), 1);
  assert_int_equal(maps_bucket_count(m), 4);

  /* allocate map with allocated components */
  maps_free(m);

  m = maps_new(sizeof value);
  maps_release(m, release_value);
  assert_true(m != NULL);
  assert_int_equal(maps_size(m), 0);

  /* insert objects */
  value = 42;
  maps_insert(m, test_strdup("foo"), &value);
  value = 4711;
  maps_insert(m, test_strdup("bar"), &value);

  /* insert collision (will collide with 'foo') */
  maps_insert(m, test_strdup("}xopjr{zsjredlz}miflwanoygrdq"), &value);

  /* insert duplicate */
  maps_insert(m, test_strdup("bar"), &value);
  assert_int_equal(maps_size(m), 3);

  /* lookup object */
  assert_true(*(int *) maps_at(m, "foo") == 42);

  /* rehash to zero */
  maps_rehash(m, 0);

  /* erase object with allocated components */
  maps_erase(m, "foo");
  assert_int_equal(maps_size(m), 2);

  maps_free(m);
}

int main()
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(core)
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
