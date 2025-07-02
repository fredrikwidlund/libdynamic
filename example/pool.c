#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <err.h>

#include "dynamic.h"

static uint64_t ntime(void)
{
  struct timespec tv;
  clock_gettime(CLOCK_MONOTONIC, &tv);
  return (uint64_t) tv.tv_sec * 1000000000ULL + (uint64_t) tv.tv_nsec;
}

void test1(size_t iterations, size_t batch, size_t size)
{
  void *m[batch];
  size_t i, j;

  for (j = 0; j < iterations; j++)
  {
    for (i = 0; i < batch; i++)
      m[i] = malloc(size);
    for (i = 0; i < batch; i++)
      free(m[i]);
  }
}

void test2(size_t iterations, size_t batch, size_t size)
{
  void *m[batch];
  size_t i, j;
  pool_t p = pool();

  for (j = 0; j < iterations; j++)
  {
    for (i = 0; i < batch; i++)
      p = pool_alloc(p, &m[i], size);
    for (i = 0; i < batch; i++)
      p = pool_return(p, m[i]);
  }
  pool_free(p);
}


int main(int argc, char **argv)
{
  uint64_t n1, n2;
  size_t iterations, batch, size;

  if (argc != 4
    )
    errx(1, "usage: pool [iterations] [batch] [size]\n");

  iterations = strtoul(argv[1], NULL, 0);
  batch = strtoul(argv[2], NULL, 0);
  size = strtoul(argv[3], NULL, 0);

  n1 = ntime();
  test1(iterations, batch, size);
  n1 = ntime() - n1;
  printf("[malloc] %fs\n", (double) n1 / 1000000000.0);

  n2 = ntime();
  test2(iterations, batch, size);
  n2 = ntime() - n2;
  printf("[pool] %fs\n", (double) n2 / 1000000000.0);

  printf("factor %f\n", (double) n1 / (double) n2);
}
