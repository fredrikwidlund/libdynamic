#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <assert.h>

#include <dynamic.h>

#include "map_str_ptr.h"

typedef struct value value;
struct value
{
  u_int number;
};

static uint64_t nano_time(void)
{
  struct timespec ts;

  (void) clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
  return ((uint64_t) ts.tv_sec * 1000000000) + ((uint64_t) ts.tv_nsec);
}

int main(int argc, char **argv)
{
  char **keys, buffer[256];
  value **values;
  map_str_ptr m;
  u_int n, i, r;
  uint64_t t1, t2;

  if (argc != 2)
    exit(1);
  n = strtoul(argv[1], NULL, 0);

  // create keys/values
  keys = calloc(n, sizeof keys[0]);
  values = calloc(n, sizeof values[0]);
  for (i = 0; i < n; i ++)
    {
      snprintf(buffer, sizeof buffer, "key-%u", i);
      keys[i] = strdup(buffer);
      values[i] = malloc(sizeof(value));
      values[i]->number = i;
    }

  // construct map
  map_str_ptr_construct(&m);

  // insert key->value mappings

  t1 = nano_time();
  for (r = 0; r < 10; r ++)
    for (i = 0; i < n; i ++)
      map_str_ptr_insert(&m, keys[i], values[i]);
  t2 = nano_time();
  printf("%lu\n", t2 - t1);

  // lookup key and validate value
  for (i = 0; i < n; i ++)
    {
      snprintf(buffer, sizeof buffer, "key-%u", i);
      assert(((value *) map_str_ptr_at(&m, buffer))->number == i);
    }

  // release keys/values
  for (i = 0; i < n; i ++)
    {
      free(keys[i]);
      free(values[i]);
    }
  free(keys);
  free(values);

  // destruct map
  map_str_ptr_destruct(&m);
}
