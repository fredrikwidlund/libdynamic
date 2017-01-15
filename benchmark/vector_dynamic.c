#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <err.h>
#include <time.h>

#include "dynamic.h"
#include "vector.h"

void vector_dynamic(vector_metric *metric, size_t n)
{
  vector v;
  size_t i;
  uint64_t t1, t2;

  vector_construct(&v, sizeof(uint32_t));

  t1 = ntime();
  for (i = 0; i < n; i ++)
    vector_push_back(&v, (uint32_t[]){i});
  t2 = ntime();
  metric->insert = (double) (t2 - t1) / n;

  vector_destruct(&v);
}
