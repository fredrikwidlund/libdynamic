#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <sys/time.h>

#define UNIT_TESTING 1

#include <setjmp.h>
#include <cmocka.h>

#include "../src/dynamic/mapi.h"

struct ii
{
  uint32_t  key;
  uint32_t  value;
};

struct is
{
  uint32_t  key;
  char     *value;
};

char *test_strdup(char *string)
{
  void *copy;

  copy = malloc(strlen(string) + 1);
  return copy ? strcpy(copy, string) : NULL;
}

void release_value(void *object)
{
  struct is *is = object;
  free(is->value);
}

void core()
{
  mapi *m;
  struct ii *ii;
  struct is *is;

  /* allocate map */
  m = mapi_new(sizeof *ii);
  assert_true(m != NULL);
  assert_int_equal(mapi_size(m), 0);
  assert_int_equal(mapi_bucket_count(m), 0);

  /* clear empty map */
  mapi_clear(m);
  assert_int_equal(mapi_size(m), 0);

  /* find in empty map */
  ii = mapi_find(m, 42);
  assert_true(ii == NULL);

  /* insert object */
  mapi_insert(m, (struct ii[]){{42, 1000}});
  assert_int_equal(mapi_size(m), 1);
  assert_int_equal(mapi_bucket_count(m), MAPI_MIN_CAPACITY);

  /* insert duplicate */
  mapi_insert(m, (struct ii[]){{42, 1000}});
  assert_int_equal(mapi_size(m), 1);

  /* insert collision */
  mapi_insert(m, (struct ii[]){{65536 + 42, 1000}});
  assert_int_equal(mapi_size(m), 2);

  /* lookup object */
  assert_int_equal(((struct ii *) mapi_at(m, 42))->value, 1000);
  assert_int_equal(((struct ii *) mapi_find(m, 42))->value, 1000);

  /* lookup non-existing object */
  assert_true(mapi_empty(m, mapi_at(m, 7)));
  assert_true(mapi_find(m, 7) == NULL);
  mapi_free(m);

  /* create int->char map */
  m = mapi_new(sizeof *is);
  mapi_release(m, release_value);
  assert_true(m != NULL);
  assert_int_equal(mapi_size(m), 0);

  /* insert objects */
  mapi_insert(m, (struct is[]){{42, test_strdup("allocated string 1")}});
  mapi_insert(m, (struct is[]){{43, test_strdup("allocated string 2")}});

  /* insert duplicate */
  mapi_insert(m, (struct is[]){{42, test_strdup("allocated string 1")}});
  assert_int_equal(mapi_size(m), 2);

  /* lookup object */
  is = mapi_at(m, 42);
  assert_true(is->key != (uint32_t) -1);
  assert_int_equal(is->key, 42);
  assert_string_equal(is->value, "allocated string 1");

  /* rehash to zero */
  mapi_rehash(m, 0);

  /* erase object with allocated components */
  mapi_erase(m, 42);
  assert_int_equal(mapi_size(m), 1);

  mapi_free(m);
}

void erase()
{
  mapi *m;
  int v, last = MAPI_MIN_CAPACITY - 1, coll1 = 1 << 16, coll2 = 1 << 17 ;

  m = mapi_new(sizeof v);
  assert_true(m);

  /* erase non-existing object */
  mapi_erase(m, 42);
  assert_int_equal(mapi_size(m), 0);

  /* erase object with duplicate */
  mapi_insert(m, (int[]){1});
  mapi_insert(m, (int[]){1 + coll1});
  mapi_erase(m, 1);
  assert_int_equal(mapi_size(m), 1);

  /* erase object with empty succ */
  mapi_clear(m);
  mapi_insert(m, (int[]){1});
  mapi_erase(m, 5);

  /* erase when w wraps */
  mapi_clear(m);
  mapi_insert(m, (int[]){last + coll1});
  mapi_insert(m, (int[]){last});
  mapi_insert(m, (int[]){last + coll2});
  mapi_erase(m, last);

  /* erase when i wraps */
  mapi_clear(m);
  mapi_insert(m, (int[]){last});
  mapi_erase(m, last);

  /* erase when i wraps  and w < i */
  mapi_clear(m);
  mapi_insert(m, (int[]){last});
  mapi_insert(m, (int[]){0});
  mapi_insert(m, (int[]){last + coll1});
  mapi_erase(m, last);

  /* erase when i wraps and w > o */
  mapi_clear(m);
  mapi_insert(m, (int[]){last - 1});
  mapi_insert(m, (int[]){last});
  mapi_insert(m, (int[]){last + coll1});
  mapi_erase(m, last - 1);
  mapi_free(m);
}

void large()
{
  mapi *m;
  uint32_t i, *x;

  /* allocate set */
  m = mapi_new(sizeof *x);
  assert_true(m != NULL);

  /* insert 1000 integers */
  for (i = 0; i < 1000; i ++)
    mapi_insert(m, &i);
  assert_int_equal(mapi_size(m), 1000);

  /* delete 500 existing and 500 non-existing integers */
  for (i = 500; i < 1500; i ++)
    mapi_erase(m, i);
  assert_int_equal(mapi_size(m), 500);

  /* lookup 500 existing and 500 non-existing integers */
  for (i = 0; i < 1000; i ++)
    assert_int_equal(*(uint32_t *) mapi_at(m, i) != (uint32_t) -1, i < 500);

  /* reinsert 500 and insert 1000 new integers */
  for (i = 500; i < 2000; i ++)
    mapi_insert(m, &i);
  assert_int_equal(mapi_size(m), 2000);

  mapi_free(m);
}

void memory()
{
  extern int debug_out_of_memory;
  mapi *m;
  int e;

  debug_out_of_memory = 1;
  m = mapi_new(sizeof(int));
  assert_false(m);
  debug_out_of_memory = 0;

  m = mapi_new(sizeof(int));
  assert_true(m);
  debug_out_of_memory = 1;
  e = mapi_insert(m, (int[]){42});
  assert_int_equal(e, -1);
  debug_out_of_memory = 0;
  mapi_free(m);
}


int main()
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(core),
    cmocka_unit_test(erase),
    cmocka_unit_test(large),
    cmocka_unit_test(memory)
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
