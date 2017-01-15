#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "dynamic.h"

#include "map.h"
#include "map_custom.h"
#include "map_subclass.h"
#include "map_unordered.h"
#include "map_dynamic.h"

static map_metric metrics[] = {
  {.name = "custom open addressing", .measure = map_custom},
  {.name = "libdynamic", .measure = map_dynamic},
  {.name = "std::map_unordered", .measure = map_unordered},
  {.name = "libdynamic (subclass)", .measure = map_subclass}
};

uint64_t ntime(void)
{
  struct timespec ts;

  (void) clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
  return ((uint64_t) ts.tv_sec * 1000000000) + ((uint64_t) ts.tv_nsec);
}

int main()
{
  size_t i, n, n_min = 10, n_max = 1000000;
  double k = 1.1;
  uint32_t *a;
  map_metric *m;

  (void) fprintf(stdout, "name,size,insert,at,erase\n");
  for (n = n_min; n < n_max; n = ceil(k * n))
    {
      a = calloc(n, sizeof *a);
      for (i = 0; i < n; i ++)
        a[i] = rand();

      for (m = metrics; m < &metrics[sizeof metrics / sizeof metrics[0]]; m ++)
        {
          m->measure(m, a, n);
          (void) fprintf(stdout, "%s,%lu,%f,%f,%f\n", m->name, n, m->insert, m->at, m->erase);
        }

      free(a);
    }
}
