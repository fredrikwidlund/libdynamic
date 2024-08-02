#include <stdio.h>
#include <time.h>
#include <assert.h>

#include <dynamic.h>

static uint64_t ntime(void)
{
  struct timespec tv;
  clock_gettime(CLOCK_MONOTONIC, &tv);
  return (uint64_t) tv.tv_sec * 1000000000ULL + (uint64_t) tv.tv_nsec;
}

int main(int argc, char **argv)
{
  uint64_t t1, t2;
  size_t i, n;
  char c;

  n = argc == 2 ? strtoull(argv[1], NULL, 0) : 1000000000;

  vector_t v = vector();
  t1 = ntime();
  for (i = 0; i < n; i++)
  {
    c = i;
    vector_push(&v, char, c);
  }
  t2 = ntime();
  assert(vector_at(v, char, 0) == 0);
  fprintf(stderr, "%lu inserts, time %fs\n", vector_size(v), (double) (t2 - t1) / 1000000000.0);
  vector_clear(&v);
}
