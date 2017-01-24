#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "dynamic.h"

#include "map_std_unordered.h"
#include "map_custom.h"
#include "map_libdynamic.h"
#include "map_libdynamic_subclass.h"

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
  {.name = "libdynamic (subclass)", .measure = map_libdynamic_subclass}
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

static int set_int_empty = -1;

static size_t set_int_hash(map *m, void *e)
{
  (void) m;
  return *(int *) e;
}

static int set_int_equal(map *m, void *e1, void *e2)
{
  (void) m;
  return *(int *) e1 == *(int *) e2;
}

static void set_int_set(map *m, void *e1, void *e2)
{
  (void) m;
  *(int *) e1 = *(int *) e2;
}

static void input_construct(input *input, size_t size)
{
  map m;

  input->size = 0;
  input->keys = malloc(size * sizeof input->keys[0]);
  input->keys_shuffled = malloc(size * sizeof input->keys_shuffled[0]);
  input->values = malloc(size * sizeof input->values[0]);

  map_construct(&m, sizeof(int), &set_int_empty, set_int_set);

  while (input->size < size)
    {
      input->keys[input->size] = random();
      if (*(int *) map_at(&m, &input->keys[input->size], set_int_hash, set_int_equal) != -1)
        continue;
      map_insert(&m, &input->keys[input->size], set_int_hash, set_int_equal, set_int_set, NULL);

      input->keys_shuffled[input->size] = input->keys[input->size];
      input->values[input->size] = random();
      input->size ++;
    }

  map_destruct(&m, NULL, NULL);

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
  size_min = 100;
  size_max = 1000000;
  size_factor = 1.1;
  rounds = 5;
  lookups = 100000;

  vector_construct(&metrics, sizeof metric);

  /* iterate through libraries */
  for (size = size_max; size >= size_min; size = floor(size / size_factor))
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
  (void) fprintf(stdout, "name,size,insert,lookup,delete,checksum\n");
  for (i = 0; i < vector_size(&metrics); i ++)
    {
      mp = vector_at(&metrics, i);
      (void) fprintf(stdout, "%s,%lu,%f,%f,%f,%lu\n", mp->name, mp->size, mp->insert, mp->lookup, mp->delete, mp->sum);
    }

  vector_destruct(&metrics);
}
