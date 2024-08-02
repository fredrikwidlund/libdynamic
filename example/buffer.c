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

  buffer_t b = buffer();
  t1 = ntime();
  for (i = 0; i < n; i++)
  {
    c = i;
    buffer_append(&b, (data_t) {&c, 1});
  }
  t2 = ntime();
  assert(*(char *) buffer_base(b) == 0);
  fprintf(stderr, "%lu inserts from 0, time %fs\n", buffer_size(b), (double) (t2 - t1) / 1000000000);

  buffer_resize(&b, 0);
  t1 = ntime();
  for (i = 0; i < n; i++)
  {
    c = i;
    buffer_append(&b, (data_t) {&c, 1});
  }
  t2 = ntime();
  assert(*(char *) buffer_base(b) == 0);
  fprintf(stderr, "%lu inserts when already allocated, time %fs\n", buffer_size(b), (double) (t2 - t1) / 1000000000);

    buffer_resize(&b, 0);
  t1 = ntime();
  for (i = 0; i < n; i++)
  {
    c = i;
    buffer_prepend(&b, (data_t) {&c, 1});
  }
  t2 = ntime();
  fprintf(stderr, "%lu inserts in front when already allocated, time %fs\n", buffer_size(b), (double) (t2 - t1) / 1000000000);

}
