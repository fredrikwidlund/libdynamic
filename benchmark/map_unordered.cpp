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

  m.max_load_factor(0.5);
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
  size_t max = 10000000;
  uint32_t s, i, n, *r;
  double insert, at;

  (void) fprintf(stdout, "size,insert,at\n");
  for (s = 1000; s < max; s *= 1.1)
    {
      r = (uint32_t *) malloc(s * sizeof n);
      for (i = 0; i < s; i ++)
        r[i] = rand();

      unordered_map_insert(r, s, &insert, &at);

      free(r);
      (void) fprintf(stdout, "%u,%f,%f\n", s, insert * 1000000000 / (double) s, at * 1000000000 / (double) s);
    }
}
