#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <err.h>
#include <time.h>

#include "dynamic.h"
#include "map.h"

typedef struct element element;
struct element
{
  uint32_t key;
  uint32_t value;
};

void map_dynamic_int(map_metric *metric, uint32_t *a, size_t n)
{
  map_int m;
  element *e;
  size_t i;
  uint64_t t1, t2;

  map_int_construct(&m, sizeof *e);

  t1 = ntime();
  for (i = 0; i < n; i ++)
    map_int_insert(&m, (element[]) {{.key = a[i], .value = 1}});
  t2 = ntime();
  metric->insert = (double) (t2 - t1) / n;

  t1 = ntime();
  for (i = 0; i < n; i ++)
    if (((element *) map_int_at(&m, a[i]))->value != 1)
      abort();
  t2 = ntime();
  metric->at = (double) (t2 - t1) / n;

  t1 = ntime();
  for (i = 0; i < n; i ++)
    map_int_erase(&m, a[i]);
  t2 = ntime();
  if (map_int_size(&m))
    errx(1, "inconsistency");
  metric->erase = (double) (t2 - t1) / n;

  map_int_destruct(&m);
}
