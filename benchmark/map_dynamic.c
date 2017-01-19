#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
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

void map_dynamic(map_metric *metric, uint32_t *a, size_t n)
{
  map m;
  map_element *e;
  size_t i;
  uint64_t t1, t2;

  map_construct(&m, sizeof *e, (map_element[]) {{.key = -1}}, set);

  t1 = ntime();
  for (i = 0; i < n; i ++)
    map_insert(&m, (map_element[]) {{.key = a[i], .value = 1}}, hash, equal, set, NULL);
  t2 = ntime();
  metric->insert = (double) (t2 - t1) / n;

  t1 = ntime();
  for (i = 0; i < n; i ++)
    if (((map_element *) map_at(&m, (map_element[]){{.key = a[i]}}, hash, equal))->value != 1)
      abort();
  t2 = ntime();
  metric->at = (double) (t2 - t1) / n;

  t1 = ntime();
  for (i = 0; i < n; i ++)
    map_erase(&m, (map_element[]){{.key = a[i]}}, hash, equal, set, NULL);
  t2 = ntime();
  if (map_size(&m))
    errx(1, "inconsistency");
  metric->erase = (double) (t2 - t1) / n;

  map_destruct(&m, NULL, NULL);
}
