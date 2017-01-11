#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include "dynamic.h"

struct e {
  uint32_t k;
  uint32_t v;
};

size_t hash(void *e)
{
  return ((struct e *) e)->k;
}

int equal(void *e1, void *e2)
{
  return ((struct e *) e1)->k == ((struct e *) e2)->k;
}

uint64_t ntime()
{
  struct timespec ts;

  (void) clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
  return ((uint64_t) ts.tv_sec * 1000000000) + ((uint64_t) ts.tv_nsec);
}

void dynamic_map_insert(uint32_t *a, size_t n, double *insert, double *at)
{
  map m;
  size_t i;
  uint64_t t1, t2;

  map_construct(&m, sizeof (struct e), (struct e[]) {{-1, 0}});
  t1 = ntime();
  for (i = 0; i < n; i ++)
    map_insert(&m, (struct e[]) {{.k = a[i], .v = 1}}, hash, equal, NULL);
  t2 = ntime();
  *insert = (double) (t2 - t1) / 1000000000.;

  t1 = ntime();
  for (i = 0; i < n; i ++)
    if (((struct e *) map_at(&m, (struct e[]){{.k = a[i]}}, hash, equal))->v != 1)
      abort();
  t2 = ntime();
  *at = (double) (t2 - t1) / 1000000000.;

  map_destruct(&m, NULL, NULL);
}

int main()
{
  map m;
  uint32_t s, i, n, *r, max = 10000000;
  double insert, at;

  (void) fprintf(stdout, "size,insert,at\n");
  for (s = 100; s < max; s *= 1.2)
    {
      r = malloc(s * sizeof n);
      for (i = 0; i < s; i ++)
        r[i] = rand();

      map_construct(&m, sizeof n, (uint32_t[]) {-1});
      dynamic_map_insert(r, s, &insert, &at);

      free(r);
      (void) fprintf(stdout, "%u,%f,%f\n", s, insert * 1000000000 / (double) s, at * 1000000000 / (double) s);
    }
}
