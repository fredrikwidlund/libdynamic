#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>

#include "dynamic/map.h"

static size_t hash(const void *element)
{
  return *(int *) element;
}

static int empty(const void *element)
{
  return *(int *) element == 0;
}

static int equal(const void *element, const void *slot)
{
  return *(int *) element == *(int *) slot;
}

static void release(void *element)
{
  (void) element;
}

static const map_type_t map_type =
{
  .null = (int []) {0},
  .size = sizeof (int),
  .hash = hash,
  .empty = empty,
  .equal = equal
};

static const map_type_t map_type2 =
{
  .null = (int []) {0},
  .size = sizeof (int),
  .hash = hash,
  .empty = empty,
  .equal = equal,
  .release = release
};

void test_map(__attribute__((unused)) void **arg)
{
  map_t m;
  int *i;
  uint32_t last, coll1 = 1 << 16, coll2 = 1 << 17;

  assert_int_equal(map_roundup(0), 0);
  assert_int_equal(map_roundup(1), 1);

  m = map(map_type);
  i = map_lookup(&m, map_type, (int []){42});
  assert_true(map_type.empty(i));

  map_insert(&m, map_type, (int []){0});
  map_insert(&m, map_type, (int []){42});
  map_insert(&m, map_type, (int []){42});
  map_insert(&m, map_type2, (int []){42});
  map_insert(&m, map_type, (int []){4711});
  map_insert(&m, map_type, (int []){13});
  map_insert(&m, map_type, (int []){7});
  map_insert(&m, map_type, (int []){1});

  i = map_lookup(&m, map_type, (int []){42});
  assert_int_equal(*i, 42);
  map_erase(&m, map_type2, (int []){42});
  i = map_lookup(&m, map_type, (int []){43});
  assert_int_equal(*i, 0);

  assert_int_equal(map_size(&m), 4);
  map_clear(&m, map_type);

  /* erase non-existing object */
  m = map(map_type);
  map_erase(&m, map_type, (uint32_t[]) {42});
  assert_int_equal(map_size(&m), 0);

  /* erase object with duplicate */
  map_insert(&m, map_type, (uint32_t[]) {1});
  map_insert(&m, map_type, (uint32_t[]) {1 + coll1});
  map_erase(&m, map_type, (uint32_t[]) {1});
  assert_int_equal(map_size(&m), 1);
  map_clear(&m, map_type);

  /* erase object with empty succ */
  m = map(map_type);
  map_insert(&m, map_type, (uint32_t[]) {1});
  map_erase(&m, map_type, (uint32_t[]) {5});
  assert_false(empty(map_lookup(&m, map_type, (uint32_t[]) {1})));
  map_clear(&m, map_type);

  /* erase when w wraps */
  m = map(map_type);
  map_rehash(&m, map_type, 8);
  last = m.capacity - 1;
  map_insert(&m, map_type, (uint32_t[]) {last + coll1});
  map_insert(&m, map_type, (uint32_t[]) {last});
  map_insert(&m, map_type, (uint32_t[]) {last + coll2});
  map_erase(&m, map_type, (uint32_t[]) {last});
  assert_false(empty(map_lookup(&m, map_type, (uint32_t[]) {last + coll1})));
  assert_false(empty(map_lookup(&m, map_type, (uint32_t[]) {last + coll2})));
  map_clear(&m, map_type);

  /* erase when i wraps */
  m = map(map_type);
  map_rehash(&m, map_type, 8);
  last = m.capacity - 1;
  map_insert(&m, map_type, (uint32_t[]) {last});
  map_erase(&m, map_type, (uint32_t[]) {last});
  assert_true(empty(map_lookup(&m, map_type, (uint32_t[]) {last})));
  map_clear(&m, map_type);

   /* erase when i wraps and w < i */
  m = map(map_type);
  map_rehash(&m, map_type, 8);
  last = m.capacity - 1;
  map_insert(&m, map_type, (uint32_t[]) {last});
  map_insert(&m, map_type, (uint32_t[]) {coll1});
  map_insert(&m, map_type, (uint32_t[]) {last + coll1});
  map_erase(&m, map_type, (uint32_t[]) {last});
  assert_false(empty(map_lookup(&m, map_type, (uint32_t[]) {coll1})));
  assert_false(empty(map_lookup(&m, map_type, (uint32_t[]) {last + coll1})));
  assert_true(empty(map_lookup(&m, map_type, (uint32_t[]) {last})));
  map_clear(&m, map_type);

  /* erase when i wraps and w > o */
  m = map(map_type);
  map_rehash(&m, map_type, 8);
  last = m.capacity - 1;
  map_insert(&m, map_type, (uint32_t[]) {last - 1});
  map_insert(&m, map_type, (uint32_t[]) {last});
  map_insert(&m, map_type, (uint32_t[]) {last + coll1});
  map_erase(&m, map_type, (uint32_t[]) {last - 1});
  assert_false(empty(map_lookup(&m, map_type, (uint32_t[]) {last})));
  assert_false(empty(map_lookup(&m, map_type, (uint32_t[]) {last + coll1})));
  assert_true(empty(map_lookup(&m, map_type, (uint32_t[]) {last - 1})));
  map_clear(&m, map_type);

  /* erase when j wraps */
  m = map(map_type);
  map_rehash(&m, map_type, 8);
  last = m.capacity - 1;
  map_insert(&m, map_type, (uint32_t[]) {last});
  map_insert(&m, map_type, (uint32_t[]) {coll1});
  map_insert(&m, map_type, (uint32_t[]) {coll2});
  map_erase(&m, map_type, (uint32_t[]) {last});
  assert_false(empty(map_lookup(&m, map_type, (uint32_t[]) {coll1})));
  assert_false(empty(map_lookup(&m, map_type, (uint32_t[]) {coll2})));
  assert_true(empty(map_lookup(&m, map_type, (uint32_t[]) {last})));
  map_clear(&m, map_type2);
}

int main()
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_map)
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
