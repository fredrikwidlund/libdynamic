#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>
#include <err.h>

#include "buffer.h"
#include "vector.h"

uint64_t ntime()
{
  struct timeval tv;

  (void) gettimeofday(&tv, NULL);
  return ((uint64_t) tv.tv_sec * 1000000000) + ((uint64_t) tv.tv_usec * 1000);
}

int main(int argc, char **argv)
{
  int i, j, e, n = strtol(argv[1], NULL, 0);
  uint64_t t, m[101];
  vector *v;

  v = vector_new(sizeof i);
  if (!v)
    err(1, "vector_new");

  t = ntime();
  m[0] = 0;
  for (i = 0; i < n; i += n / 100)
    {
      for (j = 0; j < n / 100; j ++)
        {
          e = vector_push_back(v, &i);
          if (e == -1)
            err(1, "vector_push_back");
        }
      m[i / (n / 100) + 1] = ntime() - t;
    }

  (void) fprintf(stdout, "\"size\",\"time\"\n");
  for (i = 0; i <= 100; i ++)
    (void) fprintf(stdout, "%d,%f\n", i * (n / 100), (float) m[i] / 1000000000);

  vector_free(v);  

  exit(0);
}
