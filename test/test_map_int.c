#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <sys/time.h>
#include <setjmp.h>
#include <cmocka.h>

#include "../src/map_int.h"

void release_value(void *object)
{
  free(*(void **) object);
}

void core()
{
  map_int *m;
  int value;
  char *string;

  /* allocate map */
  m = map_int_new(sizeof value, UINT32_MAX, UINT32_MAX - 1);
  assert_true(m != NULL);
  assert_int_equal(map_int_size(m), 0);
  assert_int_equal(map_int_bucket_count(m), 0);

  /* clear empty map */
  map_int_clear(m);
  assert_int_equal(map_int_size(m), 0);
  
  /* insert object */
  value = 1000;
  map_int_insert(m, 42, &value);
  assert_int_equal(map_int_size(m), 1);
  assert_int_equal(map_int_bucket_count(m), 2);

  /* insert duplicate */
  map_int_insert(m, 42, &value);
  assert_int_equal(map_int_size(m), 1);

  /* insert collision */
  map_int_insert(m, 42 + 65536, &value);
  assert_int_equal(map_int_size(m), 2);
  
  /* lookup object */
  assert_int_equal(*(int *) map_int_at(m, 42), 1000);
  
  /* lookup non-existing object */
  assert_true(map_int_find(m, 7) == map_int_end(m));
  assert_true(map_int_at(m, 7) == NULL);
  
  /* erase non-existing object */
  map_int_erase(m, 7);
  assert_int_equal(map_int_size(m), 2);

  /* erase object */
  map_int_erase(m, 42);
  assert_int_equal(map_int_size(m), 1);
  
  /* insert on deleted position */
  map_int_rehash(m, 4);
  assert_int_equal(map_int_size(m), 1);
  assert_int_equal(map_int_bucket_count(m), 4);

  /* force rehash due to deleted buckets */
  map_int_erase(m, 42 + 65536);
  assert_int_equal(map_int_size(m), 0);
  map_int_rehash(m, 4);
  assert_int_equal(map_int_bucket_count(m), 4);
  
  /* allocate map with allocated components */
  map_int_free(m);
 
  m = map_int_new(sizeof(string), UINT32_MAX, UINT32_MAX - 1);
  map_int_release(m, release_value);
  assert_true(m != NULL);
  assert_int_equal(map_int_size(m), 0);
  
  /* insert objects */
  string = malloc(128);
  strcpy(string, "allocated string");
  map_int_insert(m, 42, &string);
  string = malloc(128);
  strcpy(string, "allocated string2");
  map_int_insert(m, 43, &string);

  /* insert duplicate */
  string = malloc(128);
  strcpy(string, "allocated string");
  map_int_insert(m, 42, &string);
  assert_int_equal(map_int_size(m), 2);

  /* lookup object */
  assert_string_equal("allocated string", *(char **) map_int_at(m, 42));

  /* rehash to zero */
  map_int_rehash(m, 0);
  
  /* erase object with allocated components */
  map_int_erase(m, 42);
  assert_int_equal(map_int_size(m), 1);
  
  map_int_free(m);
}

void large()
{
  map_int *m;
  uint32_t x;

  /* allocate set */
  m = map_int_new(0, UINT32_MAX, UINT32_MAX - 1);
  assert_true(m != NULL);
  
  /* insert 1000 integers */
  for (x = 0; x < 1000; x ++)
    map_int_insert(m, x, NULL);
  assert_int_equal(map_int_size(m), 1000);

  /* delete 500 existing and 500 non-existing integers */
  for (x = 500; x < 1500; x ++)
    map_int_erase(m, x);
  assert_int_equal(map_int_size(m), 500);

  /* lookup 500 existing and 500 non-existing integers */
  for (x = 0; x < 1000; x ++)
    {
      assert_int_equal(map_int_at(m, x) != NULL, x < 500);
      assert_int_equal(map_int_find(m, x) != map_int_end(m), x < 500);
    }

  /* reinsert 500 and insert 1000 new integers */
  for (x = 500; x < 2000; x ++)
    map_int_insert(m, x, NULL);
  assert_int_equal(map_int_size(m), 2000);
  
  map_int_free(m);
}

int main()
{
  const UnitTest tests[] = {
    unit_test(core),
    unit_test(large)
  };

  return run_tests(tests);
}
