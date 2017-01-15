#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <err.h>

#include <unordered_map>

extern "C" {
#include "map.h"
void map_unordered(map_metric *, uint32_t *, size_t);
}

void map_unordered(map_metric *metric, uint32_t *a, size_t n)
{
  std::unordered_map<uint32_t, uint32_t> m;
  std::unordered_map<uint32_t, uint32_t>::iterator iter;
  size_t i;
  uint64_t t1, t2;

  m.max_load_factor(0.5);

  t1 = ntime();
  for (i = 0; i < n; i ++)
    m[a[i]] = 1;
  t2 = ntime();
  metric->insert = (double) (t2 - t1) / n;

  t1 = ntime();
  for (i = 0; i < n; i ++)
    if (m[a[i]] != 1)
      errx(1, "inconsistency");
  t2 = ntime();
  metric->at = (double) (t2 - t1) / n;

  t1 = ntime();
  for (i = 0; i < n; i ++)
    {
      iter = m.find(a[i]);
      if (iter != m.end())
        m.erase(iter);
    }
  if (m.size())
      errx(1, "inconsistency");
  t2 = ntime();
  metric->erase = (double) (t2 - t1) / n;

  m.clear();
}
