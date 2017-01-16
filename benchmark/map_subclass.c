#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
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

static size_t hash(void *e)
{
  return *(uint32_t *) e;
}

static void set(void *dst, void *src)
{
  *(uint64_t *) dst = *(uint64_t *) src;
}

static int equal(void *e1, void *e2)
{
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

void map_subclass(map_metric *metric, uint32_t *a, size_t n)
{
  map_int_pair m;
  uint64_t t1, t2;
  size_t i;

  map_int_pair_construct(&m);

  t1 = ntime();
  for (i = 0; i < n; i ++)
    map_int_pair_insert(&m, a[i], 1);
  t2 = ntime();
  metric->insert = (double) (t2 - t1) / n;

  t1 = ntime();
  for (i = 0; i < n; i ++)
    if (map_int_pair_at(&m, a[i]) != 1)
      errx(1, "inconsistency");
  t2 = ntime();
  metric->at = (double) (t2 - t1) / n;

  t1 = ntime();
  for (i = 0; i < n; i ++)
    map_int_pair_erase(&m, a[i]);
  t2 = ntime();
  if (map_int_pair_size(&m))
    errx(1, "inconsistency");
  metric->erase = (double) (t2 - t1) / n;


  map_int_pair_destruct(&m);
}
