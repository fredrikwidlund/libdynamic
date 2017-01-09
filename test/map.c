#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <sys/time.h>
#include <setjmp.h>
#include <cmocka.h>

#include "../src/dynamic/hash.h"
#include "../src/dynamic/map.h"

typedef struct element element;
struct element
{
  uint32_t  key;
  char     *value;
};

static size_t hash(void *e)
{
  return ((element *) e)->key;
}

static int equal(void *e1, void *e2)
{
  return *(uint32_t *) e1 == *(uint32_t *) e2;
}

static uint64_t string_hash(void *s)
{
  return hash_string(*(char **) s);
}

static int string_equal(void *e1, void *e2)
{
  char *s1, *s2;

  s1 = *(char **) e1;
  s2 = *(char **) e2;
  return s1 == s2 || (s1 && s2 && strcmp(s1, s2) == 0);
}

static void string_release(void *e)
{
  free(*(char **) e);
}

void core()
{
  map m;
  element *e;
  size_t i;
  char *s;

  /* insert/at */
  map_construct(&m, sizeof *e, (element[]){{.key = 0}});
  e = map_at(&m, (element[]){{.key = 4711}}, hash, equal);
  assert_true(equal(e, map_element_empty(&m)));
  map_insert(&m, (element[]){{.key = 16, .value = "value"}}, hash, equal, NULL);
  e = map_at(&m, (element[]){{.key = 16}}, hash, equal);
  assert_string_equal(((element *) e)->value, "value");
  map_insert(&m, (element[]){{.key = 32, .value = "value2"}}, hash, equal, NULL);

  /* duplicate insert should not go through */
  map_insert(&m, (element[]){{.key = 32, .value = "value3"}}, hash, equal, NULL);
  e = map_at(&m, (element[]){{.key = 32}}, hash, equal);
  assert_string_equal(((element *) e)->value, "value2");
  map_destruct(&m, equal, NULL);

  /* insert/erase */
  map_construct(&m, sizeof(uint32_t), (uint32_t[]){0});
  for (i = 500; i < 1500; i ++)
    map_insert(&m, (uint32_t[]){i}, hash, equal, NULL);
  for (i = 500; i < 1500; i ++)
    assert_false(equal(map_at(&m, (uint32_t[]){i}, hash, equal), map_element_empty(&m)));
  assert_int_equal(map_size(&m), 1000);
  for (i = 500; i < 1500; i ++)
    map_erase(&m, (uint32_t[]){i}, hash, equal, NULL);

  /* erase nonexistent */
  map_erase(&m, (uint32_t[]){42}, hash, equal, NULL);
  assert_int_equal(map_size(&m), 0);
  map_destruct(&m, equal, NULL);

  /* release */
  map_construct(&m, sizeof(char *), (char*[]){NULL});
  s = strdup("test");
  map_insert(&m, &s, string_hash, string_equal, string_release);

  /* duplicate insert should release new element */
  s = strdup("test");
  map_insert(&m, &s, string_hash, string_equal, string_release);

  /* lookup */
  s = *(char **) map_at(&m, (char*[]){"test"}, string_hash, string_equal);
  assert_string_equal(s, "test");

  /* erase should release */
  s = strdup("test2");
  map_insert(&m, &s, string_hash, string_equal, string_release);
  map_erase(&m, (char*[]){"test2"}, string_hash, string_equal, string_release);
  assert_int_equal(map_size(&m), 1);

  /* cleanup */
  map_clear(&m, string_equal, string_release);
  map_destruct(&m, string_equal, string_release);
}

void erase()
{
  map m;
  uint32_t last = MAP_ELEMENTS_CAPACITY_MIN - 1, coll1 = 1 << 16, coll2 = 1 << 17 ;

  map_construct(&m, sizeof(uint32_t), (uint32_t[]){-1});

  /* erase non-existing object */
  map_erase(&m, (uint32_t[]){42}, hash, equal, NULL);
  assert_int_equal(map_size(&m), 0);

  /* erase object with duplicate */
  map_insert(&m, (uint32_t[]){1}, hash, equal, NULL);
  map_insert(&m, (uint32_t[]){1 + coll1}, hash, equal, NULL);
  map_erase(&m, (uint32_t[]){1}, hash, equal, NULL);
  assert_int_equal(map_size(&m), 1);

  /* erase object with empty succ */
  map_clear(&m, equal, NULL);
  map_insert(&m, (uint32_t[]){1}, hash, equal, NULL);
  map_erase(&m, (uint32_t[]){5}, hash, equal, NULL);
  assert_false(equal(map_at(&m, (uint32_t[]){1}, hash, equal), map_element_empty(&m)));

  /* erase when w wraps */
  map_clear(&m, equal, NULL);
  map_insert(&m, (uint32_t[]){last + coll1}, hash, equal, NULL);
  map_insert(&m, (uint32_t[]){last}, hash, equal, NULL);
  map_insert(&m, (uint32_t[]){last + coll2}, hash, equal, NULL);
  map_erase(&m, (uint32_t[]){last}, hash, equal, NULL);
  assert_false(equal(map_at(&m, (uint32_t[]){last + coll1}, hash, equal), map_element_empty(&m)));
  assert_false(equal(map_at(&m, (uint32_t[]){last + coll2}, hash, equal), map_element_empty(&m)));

  /* erase when i wraps */
  map_clear(&m, equal, NULL);
  map_insert(&m, (uint32_t[]){last}, hash, equal, NULL);
  map_erase(&m, (uint32_t[]){last}, hash, equal, NULL);
  assert_true(equal(map_at(&m, (uint32_t[]){last}, hash, equal), map_element_empty(&m)));

  /* erase when i wraps  and w < i */
  map_clear(&m, equal, NULL);
  map_insert(&m, (uint32_t[]){last}, hash, equal, NULL);
  map_insert(&m, (uint32_t[]){0}, hash, equal, NULL);
  map_insert(&m, (uint32_t[]){last + coll1}, hash, equal, NULL);
  map_erase(&m, (uint32_t[]){last}, hash, equal, NULL);
  assert_false(equal(map_at(&m, (uint32_t[]){0}, hash, equal), map_element_empty(&m)));
  assert_false(equal(map_at(&m, (uint32_t[]){last + coll1}, hash, equal), map_element_empty(&m)));
  assert_true(equal(map_at(&m, (uint32_t[]){last}, hash, equal), map_element_empty(&m)));

  /* erase when i wraps and w > o */
  map_clear(&m, equal, NULL);
  map_insert(&m, (uint32_t[]){last - 1}, hash, equal, NULL);
  map_insert(&m, (uint32_t[]){last}, hash, equal, NULL);
  map_insert(&m, (uint32_t[]){last + coll1}, hash, equal, NULL);
  map_erase(&m, (uint32_t[]){last - 1}, hash, equal, NULL);
  assert_false(equal(map_at(&m, (uint32_t[]){last}, hash, equal), map_element_empty(&m)));
  assert_false(equal(map_at(&m, (uint32_t[]){last + coll1}, hash, equal), map_element_empty(&m)));
  assert_true(equal(map_at(&m, (uint32_t[]){last - 1}, hash, equal), map_element_empty(&m)));

  /* erase when j wraps and */
  map_clear(&m, equal, NULL);
  map_insert(&m, (uint32_t[]){last}, hash, equal, NULL);
  map_insert(&m, (uint32_t[]){0}, hash, equal, NULL);
  map_insert(&m, (uint32_t[]){0 + coll1}, hash, equal, NULL);
  map_erase(&m, (uint32_t[]){last}, hash, equal, NULL);
  assert_false(equal(map_at(&m, (uint32_t[]){0}, hash, equal), map_element_empty(&m)));
  assert_false(equal(map_at(&m, (uint32_t[]){0 + coll1}, hash, equal), map_element_empty(&m)));
  assert_true(equal(map_at(&m, (uint32_t[]){last}, hash, equal), map_element_empty(&m)));

  map_destruct(&m, equal, NULL);
}

int main()
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(core),
    cmocka_unit_test(erase),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
