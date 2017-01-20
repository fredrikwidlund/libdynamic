#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <sys/param.h>
#include <err.h>

#include <unordered_map>

#include "map_std_unordered.h"

extern "C" {
#include "map.h"
}

void map_std_unordered(int *keys, int *keys_shuffled, int *values, size_t size, size_t lookups, double *insert, double *lookup, double *erase, uint64_t *sum)
{
  std::unordered_map<int, int> m;
  std::unordered_map<int, int>::iterator iter;
  size_t i, n;
  uint64_t t1, t2, s;

  m.max_load_factor(0.5);

  t1 = ntime();
  for (i = 0; i < size; i ++)
    m[keys[i]] = values[i];
  t2 = ntime();
  *insert = (double) (t2 - t1) / size;

  s = 0;
  t1 = ntime();
  for (n = lookups; n; n -= i)
    for (i = 0; i < MIN(size, n); i ++)
      s += m[keys_shuffled[i]];
  t2 = ntime();
  *sum = s;
  *lookup = (double) (t2 - t1) / lookups;

  t1 = ntime();
  for (i = 0; i < size; i ++)
    {
      iter = m.find(keys[i]);
      if (iter != m.end())
        m.erase(iter);
    }
  t2 = ntime();
  if (m.size())
      errx(1, "inconsistency");
  *erase = (double) (t2 - t1) / size;

  m.clear();
}
