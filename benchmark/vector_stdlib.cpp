#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <err.h>

#include <vector>

extern "C" {
#include "vector.h"
void vector_stdlib(vector_metric *, size_t);
}

void vector_stdlib(vector_metric *metric, size_t n)
{
  std::vector<uint32_t> v;
  size_t i;
  uint64_t t1, t2;

  t1 = ntime();
  for (i = 0; i < n; i ++)
    v.push_back(i);
  t2 = ntime();
  metric->insert = (double) (t2 - t1) / n;

  v.clear();
}
