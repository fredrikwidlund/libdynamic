#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "dynamic.h"

//#include "map.h"
//#include "map_subclass.h"
#include "map_std_unordered.h"
#include "map_custom.h"
//#include "map_dynamic.h"

typedef struct library library;
struct library
{
  char  *name;
  void (*measure)(int *, int *, int *, size_t, size_t, double *, double *, double *, uint64_t *);
};

typedef struct metric metric;
struct metric
{
  char     *name;
  size_t    size;
  double    insert;
  double    lookup;
  double    delete;
  uint64_t  sum;
};

typedef struct input input;
struct input
{
  size_t  size;
  int    *keys;
  int    *keys_shuffled;
  int    *values;
};

static library libraries[] = {
  {.name = "std::map_unordered", .measure = map_std_unordered},
  {.name = "custom open addressing", .measure = map_custom},
  {.name = "libdynamic", .measure = map_libdynamic},
  //{.name = "libdynamic map (subclass)", .measure = map_libdynamic_subclass}
};
static const size_t libraries_len = sizeof libraries / sizeof libraries[0];

uint64_t ntime(void)
{
  struct timespec ts;

  (void) clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
  return ((uint64_t) ts.tv_sec * 1000000000) + ((uint64_t) ts.tv_nsec);
}

static void shuffle(int *array, size_t n)
{
  size_t i, j;
  int t;

  if (n <= 1)
    return;

  for (i = 0; i < n - 1; i ++)
    {
      j = i + rand() / (RAND_MAX / (n - i) + 1);
      t = array[j];
      array[j] = array[i];
      array[i] = t;
    }
}

static void input_construct(input *input, size_t size)
{
  input->size = 0;
  input->keys = malloc(size * sizeof input->keys[0]);
  input->keys_shuffled = malloc(size * sizeof input->keys_shuffled[0]);
  input->values = malloc(size * sizeof input->values[0]);

  while (input->size < size)
    {
      input->keys[input->size] = random();
      input->keys_shuffled[input->size] = input->keys[input->size];
      input->values[input->size] = random();
      input->size ++;
    }

  shuffle(input->keys_shuffled, input->size);
}

static void input_destruct(input *input)
{
  free(input->keys);
  free(input->keys_shuffled);
  free(input->values);
}

static void metric_aggregate(metric *ma, metric *mi)
{
  if (!ma->insert || ma->insert > mi->insert)
    ma->insert = mi->insert;
  if (!ma->lookup || ma->lookup > mi->lookup)
    ma->lookup = mi->lookup;
  if (!ma->delete || ma->delete > mi->delete)
    ma->delete = mi->delete;
  ma->sum = mi->sum;
}

int main()
{
  size_t size, size_min, size_max, rounds, lookups, r, i;
  double size_factor;
  vector metrics;
  input input;
  metric metric, mr, *mp;

  /* benchmark settings */
  size_min = 10;
  size_max = 1000000;
  size_factor = 1.1;
  rounds = 5;
  lookups = 100000;

  vector_construct(&metrics, sizeof metric);

  /* iterate through libraries */
  for (size = size_min; size < size_max; size = ceil(size_factor * size))
    {
      input_construct(&input, size);

      for (i = 0; i < libraries_len; i ++)
        {
          metric = (struct metric) {.name = libraries[i].name, .size = input.size};
          for (r = 0; r < rounds; r ++)
            {
              libraries[i].measure(input.keys, input.keys_shuffled, input.values, input.size, lookups,
                                   &mr.insert, &mr.lookup, &mr.delete, &mr.sum);
              metric_aggregate(&metric, &mr);
            }
          vector_push_back(&metrics, &metric);
        }

      input_destruct(&input);
    }

  /* output resulting metrics */
  (void) fprintf(stdout, "name,size,insert,lookup,delete\n");
  for (i = 0; i < vector_size(&metrics); i ++)
    {
      mp = vector_at(&metrics, i);
      (void) fprintf(stdout, "%s,%lu,%f,%f,%f\n", mp->name, mp->size, mp->insert, mp->lookup, mp->delete);
    }

  vector_destruct(&metrics);
}

/*
        {
          metric = (struct metric) {.name = m->name, .size = n};
          for (round = 0; round < rounds; round ++)
            {
              m->measure(m, a, n, 6);
              if (!metric.insert || m->insert < metric.insert)
                metric.insert = m->insert;
              if (!metric.at || m->at < metric.at)
                metric.at = m->at;
              if (!metric.erase || m->erase < metric.erase)
                metric.erase = m->erase;
            }
          vector_push_back(&metrics, &metric);
        }
      free(a);
    }

}
*/
