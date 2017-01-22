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

extern int debug_out_of_memory;
extern int debug_abort;

typedef struct element element;
struct element
{
  uint32_t  key;
  char     *value;
};

static void element_set(map *m, void *o1, void *o2)
{
  element *e1 = o1, *e2 = o2;

  (void) m;
  *e1 = *e2;
}

static size_t hash(map *m, void *e)
{
  (void) m;
  return ((element *) e)->key;
}

static int equal(map *m, void *e1, void *e2)
{
  (void) m;
  return *(uint32_t *) e1 == *(uint32_t *) e2;
}

static void set(map *m, void *e1, void *e2)
{
  (void) m;
  *(uint32_t *) e1 = *(uint32_t *) e2;
}

static uint64_t string_hash(map *m, void *s)
{
  (void) m;
  return hash_string(*(char **) s);
}

static int string_equal(map *m, void *e1, void *e2)
{
  char *s1, *s2;

  (void) m;
  s1 = *(char **) e1;
  s2 = *(char **) e2;
  return s1 == s2 || (s1 && s2 && strcmp(s1, s2) == 0);
}

static void string_set(map *m, void *o1, void *o2)
{
  char **s1 = o1, **s2 = o2;

  (void) m;
  *s1 = *s2;
}

static void string_release(map *m, void *e)
{
  (void) m;
  free(*(char **) e);
}

void core()
{
  map m;
  element *e;
  size_t i;
  char *s;

  /* insert/at */
  map_construct(&m, sizeof *e, (element[]){{.key = 0}}, element_set);
  e = map_at(&m, (element[]){{.key = 4711}}, hash, equal);
  assert_true(equal(&m, e, map_element_empty(&m)));
  map_insert(&m, (element[]){{.key = 16, .value = "value"}}, hash, equal, element_set, NULL);
  e = map_at(&m, (element[]){{.key = 16}}, hash, equal);
  assert_string_equal(((element *) e)->value, "value");
  map_insert(&m, (element[]){{.key = 32, .value = "value2"}}, hash, equal, element_set, NULL);

  /* duplicate insert should not go through */
  map_insert(&m, (element[]){{.key = 32, .value = "value3"}}, hash, equal, element_set, NULL);
  e = map_at(&m, (element[]){{.key = 32}}, hash, equal);
  assert_string_equal(((element *) e)->value, "value2");
  map_destruct(&m, equal, NULL);

  /* insert/erase */
  map_construct(&m, sizeof(uint32_t), (uint32_t[]){0}, set);
  for (i = 500; i < 1500; i ++)
    map_insert(&m, (uint32_t[]){i}, hash, equal, set, NULL);
  for (i = 500; i < 1500; i ++)
    assert_false(equal(&m, map_at(&m, (uint32_t[]){i}, hash, equal), map_element_empty(&m)));
  assert_int_equal(map_size(&m), 1000);
  for (i = 500; i < 1500; i ++)
    map_erase(&m, (uint32_t[]){i}, hash, equal, set, NULL);

  /* erase nonexistent */
  map_erase(&m, (uint32_t[]){42}, hash, equal, set, NULL);
  assert_int_equal(map_size(&m), 0);
  map_destruct(&m, NULL, NULL);

  /* release */
  map_construct(&m, sizeof(char *), (char *[]){NULL}, string_set);
  s = strdup("test");
  map_insert(&m, &s, string_hash, string_equal, string_set, string_release);

  /* duplicate insert should release new element */
  s = strdup("test");
  map_insert(&m, &s, string_hash, string_equal, element_set, string_release);

  /* lookup */
  s = *(char **) map_at(&m, (char*[]){"test"}, string_hash, string_equal);
  assert_string_equal(s, "test");

  /* erase should release */
  s = strdup("test2");
  map_insert(&m, &s, string_hash, string_equal, string_set, string_release);
  map_erase(&m, (char*[]){"test2"}, string_hash, string_equal, string_set, string_release);
  assert_int_equal(map_size(&m), 1);

  /* cleanup */
  map_clear(&m, string_equal, string_set, string_release);
  assert_int_equal(map_size(&m), 0);
  map_destruct(&m, string_equal, string_release);
}

void erase()
{
  map m;
  uint32_t last = MAP_ELEMENTS_CAPACITY_MIN - 1, coll1 = 1 << 16, coll2 = 1 << 17 ;

  map_construct(&m, sizeof(uint32_t), (uint32_t[]){-1}, set);

  /* erase non-existing object */
  map_erase(&m, (uint32_t[]){42}, hash, equal, set, NULL);
  assert_int_equal(map_size(&m), 0);

  /* erase object with duplicate */
  map_insert(&m, (uint32_t[]){1}, hash, equal, set, NULL);
  map_insert(&m, (uint32_t[]){1 + coll1}, hash, equal, set, NULL);
  map_erase(&m, (uint32_t[]){1}, hash, equal, set, NULL);
  assert_int_equal(map_size(&m), 1);

  /* erase object with empty succ */
  map_clear(&m, equal, set, NULL);
  map_insert(&m, (uint32_t[]){1}, hash, equal, set, NULL);
  map_erase(&m, (uint32_t[]){5}, hash, equal, set, NULL);
  assert_false(equal(&m, map_at(&m, (uint32_t[]){1}, hash, equal), map_element_empty(&m)));

  /* erase when w wraps */
  map_clear(&m, equal, set, NULL);
  map_insert(&m, (uint32_t[]){last + coll1}, hash, equal, set, NULL);
  map_insert(&m, (uint32_t[]){last}, hash, equal, set, NULL);
  map_insert(&m, (uint32_t[]){last + coll2}, hash, equal, set, NULL);
  map_erase(&m, (uint32_t[]){last}, hash, equal, set, NULL);
  assert_false(equal(&m, map_at(&m, (uint32_t[]){last + coll1}, hash, equal), map_element_empty(&m)));
  assert_false(equal(&m, map_at(&m, (uint32_t[]){last + coll2}, hash, equal), map_element_empty(&m)));

  /* erase when i wraps */
  map_clear(&m, equal, set, NULL);
  map_insert(&m, (uint32_t[]){last}, hash, equal, set, NULL);
  map_erase(&m, (uint32_t[]){last}, hash, equal, set, NULL);
  assert_true(equal(&m, map_at(&m, (uint32_t[]){last}, hash, equal), map_element_empty(&m)));

  /* erase when i wraps  and w < i */
  map_clear(&m, equal, set, NULL);
  map_insert(&m, (uint32_t[]){last}, hash, equal, set, NULL);
  map_insert(&m, (uint32_t[]){0}, hash, equal, set, NULL);
  map_insert(&m, (uint32_t[]){last + coll1}, hash, equal, set, NULL);
  map_erase(&m, (uint32_t[]){last}, hash, equal, set, NULL);
  assert_false(equal(&m, map_at(&m, (uint32_t[]){0}, hash, equal), map_element_empty(&m)));
  assert_false(equal(&m, map_at(&m, (uint32_t[]){last + coll1}, hash, equal), map_element_empty(&m)));
  assert_true(equal(&m, map_at(&m, (uint32_t[]){last}, hash, equal), map_element_empty(&m)));

  /* erase when i wraps and w > o */
  map_clear(&m, equal, set, NULL);
  map_insert(&m, (uint32_t[]){last - 1}, hash, equal, set, NULL);
  map_insert(&m, (uint32_t[]){last}, hash, equal, set, NULL);
  map_insert(&m, (uint32_t[]){last + coll1}, hash, equal, set, NULL);
  map_erase(&m, (uint32_t[]){last - 1}, hash, equal, set, NULL);
  assert_false(equal(&m, map_at(&m, (uint32_t[]){last}, hash, equal), map_element_empty(&m)));
  assert_false(equal(&m, map_at(&m, (uint32_t[]){last + coll1}, hash, equal), map_element_empty(&m)));
  assert_true(equal(&m, map_at(&m, (uint32_t[]){last - 1}, hash, equal), map_element_empty(&m)));

  /* erase when j wraps and */
  map_clear(&m, equal, set, NULL);
  map_insert(&m, (uint32_t[]){last}, hash, equal, set, NULL);
  map_insert(&m, (uint32_t[]){0}, hash, equal, set, NULL);
  map_insert(&m, (uint32_t[]){0 + coll1}, hash, equal, set, NULL);
  map_erase(&m, (uint32_t[]){last}, hash, equal, set, NULL);
  assert_false(equal(&m, map_at(&m, (uint32_t[]){0}, hash, equal), map_element_empty(&m)));
  assert_false(equal(&m, map_at(&m, (uint32_t[]){0 + coll1}, hash, equal), map_element_empty(&m)));
  assert_true(equal(&m, map_at(&m, (uint32_t[]){last}, hash, equal), map_element_empty(&m)));

  map_destruct(&m, equal, NULL);
}

void alloc()
{
  map m;

  debug_out_of_memory = 1;
  debug_abort = 1;
  expect_assert_failure(map_construct(&m, sizeof(uint32_t), (uint32_t[]){-1}, set));
  debug_abort = 0;
  debug_out_of_memory = 0;
}

int main()
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(core),
    cmocka_unit_test(erase),
    cmocka_unit_test(alloc)
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
