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
#include "map_dynamic_int.h"

static map_metric metrics[] = {
  //  {.name = "std::map_unordered", .measure = map_unordered},
  {.name = "libdynamic (map_int)", .measure = map_dynamic_int},
  {.name = "custom open addressing", .measure = map_custom},
  {.name = "libdynamic (map)", .measure = map_dynamic},
  {.name = "libdynamic (map subclass)", .measure = map_subclass}
};

struct result
{
  char   *name;
  size_t  size;
  double  insert;
  double  at;
  double  erase;
};

uint64_t ntime(void)
{
  struct timespec ts;

  (void) clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
  return ((uint64_t) ts.tv_sec * 1000000000) + ((uint64_t) ts.tv_nsec);
}

int main()
{
  size_t i, n, n_min = 10, n_max = 1000000, round, rounds = 5;
  double k = 1.1;
  uint32_t *a;
  map_metric *m;
  vector results;
  struct result result, *r;

  vector_construct(&results, sizeof result);
  for (n = n_min; n < n_max; n = ceil(k * n))
    {
      a = calloc(n, sizeof *a);
      for (i = 0; i < n; i ++)
        a[i] = rand();

      for (m = metrics; m < &metrics[sizeof metrics / sizeof metrics[0]]; m ++)
        {
          result = (struct result) {.name = m->name, .size = n};
          for (round = 0; round < rounds; round ++)
            {
              m->measure(m, a, n);
              if (!result.insert || m->insert < result.insert)
                result.insert = m->insert;
              if (!result.at || m->at < result.at)
                result.at = m->at;
              if (!result.erase || m->erase < result.erase)
                result.erase = m->erase;
            }
          vector_push_back(&results, &result);
        }
      free(a);
    }

  (void) fprintf(stdout, "name,size,insert,at,erase\n");
  for (i = 0; i < vector_size(&results); i ++)
    {
      r = vector_at(&results, i);
      (void) fprintf(stdout, "%s,%lu,%f,%f,%f\n", r->name, r->size, r->insert, r->at, r->erase);
    }

  vector_destruct(&results);
}
