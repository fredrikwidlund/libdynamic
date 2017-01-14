#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "dynamic.h"

#include "vector.h"
#include "vector_stdlib.h"
#include "vector_dynamic.h"

static vector_metric metrics[] = {
  {.name = "libdynamic", .measure = vector_dynamic},
  {.name = "std::vector", .measure = vector_stdlib}
};

uint64_t ntime(void)
{
  struct timespec ts;

  (void) clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
  return ((uint64_t) ts.tv_sec * 1000000000) + ((uint64_t) ts.tv_nsec);
}

int main()
{
  size_t n, n_min = 1000, n_max = 100000000;
  double k = 1.1;
  vector_metric *m;

  (void) fprintf(stdout, "name,size,insert\n");
  for (n = n_min; n < n_max; n = ceil(k * n))
    {
      for (m = metrics; m < &metrics[sizeof metrics / sizeof metrics[0]]; m ++)
        {
          m->measure(m, n);
          (void) fprintf(stdout, "%s,%lu,%f\n", m->name, n, m->insert);
        }
    }
}
