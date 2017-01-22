#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/param.h>
#include <err.h>
#include <time.h>

#include "dynamic.h"
#include "map.h"

typedef struct map_element map_element;
struct map_element
{
  uint32_t key;
  uint32_t value;
};

static size_t hash(map *m, void *e)
{
  (void) m;
  return *(uint32_t *) e;
}

static int equal(map *m, void *e1, void *e2)
{
  (void) m;
  return *(uint32_t *) e1 == *(uint32_t *) e2;
}

static void set(map *m, void *e1, void *e2)
{
  (void) m;
  *(uint64_t *) e1 = *(uint64_t *) e2;
}

void map_libdynamic(int *keys, int *keys_shuffled, int *values, size_t size, size_t lookups, double *insert, double *lookup, double *erase, uint64_t *sum)
{
  map m;
  uint64_t t1, t2, s;
  size_t i, n;

  map_construct(&m, sizeof(map_element), (map_element[]) {{.key = -1}}, set);

  t1 = ntime();
  for (i = 0; i < size; i ++)
    map_insert(&m, (map_element[]) {{.key = keys[i], .value = values[i]}}, hash, equal, set, NULL);
  t2 = ntime();
  *insert = (double) (t2 - t1) / size;

  s = 0;
  t1 = ntime();
  for (n = lookups; n; n -= i)
    for (i = 0; i < MIN(size, n); i ++)
      s += ((map_element *) map_at(&m, (map_element[]){{.key = keys_shuffled[i]}}, hash, equal))->value;
  t2 = ntime();
  *sum = s;
  *lookup = (double) (t2 - t1) / lookups;

  t1 = ntime();
  for (i = 0; i < size; i ++)
    map_erase(&m, (map_element[]){{.key = keys[i]}}, hash, equal, set, NULL);
  t2 = ntime();
  if (map_size(&m))
    errx(1, "inconsistency");
  *erase = (double) (t2 - t1) / size;

  map_destruct(&m, NULL, NULL);
}
