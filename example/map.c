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

static size_t set_int_hash(const void *element)
{
  return *(int *) element;
}

static int set_int_empty(const void *element)
{
  return *(int *) element == 0;
}

static int set_int_equal(const void *element, const void *slot)
{
  return *(int *) element == *(int *) slot;
}

static const map_type_t set_int =
{
  .null = (int[]) {0},
  .size = sizeof (int),
  .hash = set_int_hash,
  .empty = set_int_empty,
  .equal = set_int_equal
};

int main(int argc, char **argv)
{
  size_t i, n;
  map_t m;
  uint64_t t1, t2;
  vector_t v;
  int x;

  n = argc == 2 ? strtoull(argv[1], NULL, 0) : 100000000;

  v = vector();
  t1 = ntime();
  while (vector_length(v, int) < n)
  {
    x = random();
    if (x)
      vector_push(&v, int, x);
  }
  t2 = ntime();
  printf("added %lu random integers to vector, time %fs\n", n, (double) (t2 - t1) / 1000000000);

  m = map(set_int);

  t1 = ntime();
  for (i = 0; i < n; i++)
    map_insert(&m, set_int, (int []){vector_at(v, int, i)});
  t2 = ntime();

  fprintf(stderr, "%lu unique inserts into map, time %fs\n", map_size(&m), (double) (t2 - t1) / 1000000000);

  t1 = ntime();
  for (i = 0; i < n; i++)
    assert(map_lookup(&m, set_int, (int []){vector_at(v, int, i)}) != 0);
  t2 = ntime();
  fprintf(stderr, "%lu lookups, time %fs\n", n, (double) (t2 - t1) / 1000000000);

  t1 = ntime();
  for (i = 0; i < n; i++)
    map_erase(&m, set_int, (int []){vector_at(v, int, i)});
  t2 = ntime();
  assert(map_size(&m) == 0);
  fprintf(stderr, "%lu erases, time %fs\n", n, (double) (t2 - t1) / 1000000000);

  map_clear(&m, set_int);
  vector_clear(&v);
}
