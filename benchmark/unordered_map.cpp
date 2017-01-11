#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <unordered_map>

uint64_t ntime()
{
  struct timespec ts;

  (void) clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
  return ((uint64_t) ts.tv_sec * 1000000000) + ((uint64_t) ts.tv_nsec);
}

void unordered_map_insert(uint32_t *a, size_t n, double *insert, double *at)
{
  std::unordered_map<uint32_t, uint32_t> m;
  size_t i;
  uint64_t t1, t2;

  t1 = ntime();
  for (i = 0; i < n; i ++)
    m.insert({{a[i], 1}});
  t2 = ntime();
  *insert = (double) (t2 - t1) / 1000000000.;

  t1 = ntime();
  for (i = 0; i < n; i ++)
    if (m[a[i]] != 1)
      abort();
  t2 = ntime();
  *at = (double) (t2 - t1) / 1000000000.;

  m.clear();
}

int main()
{
  size_t size, sizes[] = {100, 1000, 10000, 100000, 1000000, 10000000};
  uint32_t s, i, n, *r;
  double insert, at;

  for (s = 0; s < sizeof sizes / sizeof sizes[0]; s ++)
    {
      size = sizes[s];
      r = (uint32_t *) malloc(size * sizeof n);
      for (i = 0; i < size; i ++)
        r[i] = rand();

      unordered_map_insert(r, size, &insert, &at);

      free(r);
      (void) fprintf(stdout, "%lu %f %f\n", size, insert, at);
    }
}
