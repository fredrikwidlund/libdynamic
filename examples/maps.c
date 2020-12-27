#include <stdio.h>
#include <time.h>
#include <string.h>
#include <assert.h>

#include <dynamic.h>

typedef struct value value;
struct value
{
  int number;
};

static uint64_t nano_time(void)
{
  struct timespec ts;

  (void) clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
  return ((uint64_t) ts.tv_sec * 1000000000) + ((uint64_t) ts.tv_nsec);
}

static void release(maps_entry *e)
{
  (void) e;
}

int main(int argc, char **argv)
{
  char **keys, buffer[256];
  value **values;
  maps m;
  int n, i, *p;
  uint64_t t1, t2;

  if (argc != 2)
    exit(1);
  n = strtoul(argv[1], NULL, 0);

  // create keys/values
  keys = calloc(n, sizeof keys[0]);
  values = calloc(n, sizeof values[0]);
  for (i = 0; i < n; i++)
  {
    snprintf(buffer, sizeof buffer, "key-%u", i);
    keys[i] = strdup(buffer);
    values[i] = malloc(sizeof(value));
    values[i]->number = i;
  }

  // construct map
  maps_construct(&m);

  // insert key->value mappings

  t1 = nano_time();
  for (i = 0; i < n; i++)
    maps_insert(&m, keys[i], (uintptr_t) values[i], release);
  t2 = nano_time();
  printf("insert %lu\n", t2 - t1);

  // lookup key and validate value
  t1 = nano_time();
  for (i = 0; i < n; i++)
  {
    snprintf(buffer, sizeof buffer, "key-%u", i);
    p = (void *) maps_at(&m, buffer);
    assert(p);
    assert(*p == i);
  }
  t2 = nano_time();
  printf("lookup %lu\n", t2 - t1);

  t1 = nano_time();
  for (i = 0; i < n; i++)
    maps_erase(&m, keys[i], release);
  t2 = nano_time();
  printf("erase  %lu\n", t2 - t1);

  // release keys/values
  for (i = 0; i < n; i++)
  {
    free(keys[i]);
    free(values[i]);
  }
  free(keys);
  free(values);

  // destruct map
  maps_destruct(&m, release);
}
