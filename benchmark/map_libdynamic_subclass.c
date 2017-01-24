#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/param.h>
#include <time.h>
#include <err.h>

#include "dynamic.h"
#include "map.h"

typedef map map_int_pair;

typedef struct map_int_pair_element map_int_pair_element;
struct map_int_pair_element
{
  uint32_t key;
  uint32_t value;
};

static map_int_pair_element empty = {.key = -1};

static size_t hash(map *m, void *e)
{
  (void) m;
  return *(uint32_t *) e;
}

static void set(map *m, void *dst, void *src)
{
  (void) m;
  *(uint64_t *) dst = *(uint64_t *) src;
}

static int equal(map *m, void *e1, void *e2)
{
  (void) m;
  return *(uint32_t *) e1 == *(uint32_t *) e2;
}

static void map_int_pair_construct(map_int_pair *m)
{
  map_construct(m, sizeof(map_int_pair_element), &empty, set);
}

static void map_int_pair_destruct(map_int_pair *m)
{
  map_destruct(m, NULL, NULL);
}

static uint32_t map_int_pair_at(map_int_pair *m, uint32_t key)
{
  return ((map_int_pair_element *) map_at(m, (uint32_t[]){key}, hash, equal))->value;
}

static void map_int_pair_insert(map_int_pair *m, uint32_t key, uint32_t value)
{
  map_insert(m,(map_int_pair_element[]){{.key = key, .value = value}}, hash, equal, set, NULL);
}

static void map_int_pair_erase(map_int_pair *m, uint32_t key)
{
  map_erase(m,(map_int_pair_element[]){{.key = key}}, hash, equal, set, NULL);
}

static size_t map_int_pair_size(map_int_pair *m)
{
  return map_size(m);
}

void map_libdynamic_subclass(int *keys, int *keys_shuffled, int *values, size_t size, size_t lookups,
                             double *insert, double *lookup, double *erase, uint64_t *sum)
{
  map m;
  uint64_t t1, t2, s;
  size_t i, n;

  map_int_pair_construct(&m);

  t1 = ntime();
  for (i = 0; i < size; i ++)
    map_int_pair_insert(&m, keys[i], values[i]);
  t2 = ntime();
  *insert = (double) (t2 - t1) / size;

  s = 0;
  t1 = ntime();
  for (n = lookups; n; n -= i)
    for (i = 0; i < MIN(size, n); i ++)
      s += map_int_pair_at(&m, keys_shuffled[i]);
  t2 = ntime();
  *sum = s;
  *lookup = (double) (t2 - t1) / lookups;

  t1 = ntime();
  for (i = 0; i < size; i ++)
    map_int_pair_erase(&m, keys[i]);
  t2 = ntime();
  if (map_int_pair_size(&m))
    errx(1, "inconsistency");
  *erase = (double) (t2 - t1) / size;

  map_int_pair_destruct(&m);
}
