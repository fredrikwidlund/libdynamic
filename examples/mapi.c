#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <assert.h>

#include <dynamic.h>

static uint64_t nano_time(void)
{
  struct timespec ts;

  (void) clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
  return ((uint64_t) ts.tv_sec * 1000000000) + ((uint64_t) ts.tv_nsec);
}

static void release(mapi_entry *e)
{
  (void) e;
}

int main(int argc, char **argv)
{
  uintptr_t *keys, *values, p;
  mapi m;
  uint64_t n, i;
  uint64_t t1, t2;

  if (argc != 2)
    exit(1);
  n = strtoul(argv[1], NULL, 0);
  // create keys/values
  keys = calloc(n, sizeof keys[0]);
  values = calloc(n, sizeof values[0]);
  for (i = 0; i < n; i ++)
    {
      keys[i] = i + 1;
      values[i] = i + 1;
    }

  // construct map
  mapi_construct(&m);

  // insert key->value mappings

  t1 = nano_time();
  for (i = 0; i < n; i ++)
    mapi_insert(&m, keys[i], values[i], release);
  t2 = nano_time();
  printf("insert %lu\n", t2 - t1);

  // lookup key and validate value
  t1 = nano_time();
  for (i = 0; i < n; i ++)
    {
      p = mapi_at(&m, keys[i]);
      assert(p == values[i]);
    }
  t2 = nano_time();
  printf("lookup %lu\n", t2 - t1);

  t1 = nano_time();
  for (i = 0; i < n; i ++)
    mapi_erase(&m, keys[i], release);
  t2 = nano_time();
  printf("erase  %lu\n", t2 - t1);

  // release keys/values
  free(keys);
  free(values);

  // destruct map
  mapi_destruct(&m, release);
}
