#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <sys/time.h>
#include <setjmp.h>
#include <cmocka.h>

#include "../src/dynamic/map.h"
#include "../src/dynamic/maps.h"

extern int debug_out_of_memory;
extern int debug_abort;

void release(maps_entry *e)
{
  free(e->key);
}

void core()
{
  maps maps;
  int i;
  char key[16];

  maps_construct(&maps);

  maps_insert(&maps, strdup("test"), 42, release);
  maps_insert(&maps, strdup("test"), 42, release);
  assert_true(maps_at(&maps, "test") == 42);
  assert_true(maps_size(&maps) == 1);
  maps_erase(&maps, "test2", release);
  assert_true(maps_size(&maps) == 1);
  maps_erase(&maps, "test", release);
  assert_true(maps_size(&maps) == 0);

  maps_reserve(&maps, 32);
  assert_true(maps.map.elements_capacity == 64);

  for (i = 0; i < 100; i ++)
    {
      snprintf(key, sizeof key, "test%d", i);
      maps_insert(&maps, strdup(key), i, release);
    }
  maps_clear(&maps, release);
  maps_destruct(&maps, release);
}

void memory()
{
  maps maps;

  debug_out_of_memory = 1;
  debug_abort = 1;
  expect_assert_failure(maps_construct(&maps));
  debug_abort = 0;
  debug_out_of_memory = 0;
}

int main()
{
  const struct CMUnitTest tests[] =
    {
     cmocka_unit_test(core),
     cmocka_unit_test(memory),
    };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
