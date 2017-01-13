#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <err.h>

#include "map.h"

typedef struct map_element map_element;
struct map_element
{
  uint32_t key;
  uint32_t value;
};

typedef struct map map;
struct map
{
  map_element *elements;
  size_t       elements_count;
  size_t       elements_capacity;
};

static void map_reserve(map *m, size_t size);
static void map_rehash(map *m, size_t size);

static void map_construct(map *m)
{
  m->elements = NULL;
  m->elements_count = 0;
  m->elements_capacity = 0;
  map_rehash(m, 16);
}

static void map_destruct(map *m)
{
  free(m->elements);
}

static size_t map_roundup(size_t s)
{
  s --;
  s |= s >> 1;
  s |= s >> 2;
  s |= s >> 4;
  s |= s >> 8;
  s |= s >> 16;
  s |= s >> 32;
  s ++;

  return s;
}

static map_element *map_at(map *m, uint32_t key)
{
  size_t i;
  map_element *e;

  i = key;
  while (1)
    {
      i &= m->elements_capacity - 1;
      e = &m->elements[i];
      if (e->key == key || e->key == (uint32_t) -1)
        return e;
      i ++;
    }
}

static void map_insert(map *m, uint32_t key, uint32_t value)
{
  map_element *e;

  map_reserve(m, m->elements_count + 1);
  e = map_at(m, key);
  if (e->key == (uint32_t) -1)
    {
      e->key = key;
      e->value = value;
      m->elements_count ++;
    }
}

static void map_rehash(map *m, size_t size)
{
  map new;
  size_t i;

  size = map_roundup(size);
  new = *m;
  new.elements_count = 0;
  new.elements_capacity = size;
  new.elements = malloc(new.elements_capacity * sizeof(map_element));
  for (i = 0; i < new.elements_capacity; i ++)
    new.elements[i] = (map_element) {(uint32_t) -1, 0};
  if (m->elements)
    {
      for (i = 0; i < m->elements_capacity; i ++)
        if (m->elements[i].key != (uint32_t) -1)
          map_insert(&new, m->elements[i].key, m->elements[i].value);
      free(m->elements);
    }
  *m = new;
}

static void map_reserve(map *m, size_t size)
{
  size *= 2;
  if (size > m->elements_capacity)
    map_rehash(m, size);
}

void map_custom(map_metric *metric, uint32_t *a, size_t n)
{
  map m;
  uint64_t t1, t2;
  size_t i;

  map_construct(&m);

  t1 = ntime();
  for (i = 0; i < n; i ++)
    map_insert(&m, a[i], 1);
  t2 = ntime();
  metric->insert = (double) (t2 - t1) / n;

  t1 = ntime();
  for (i = 0; i < n; i ++)
    if (map_at(&m, a[i])->value != 1)
      errx(1, "inconsistency");
  t2 = ntime();
  metric->at = (double) (t2 - t1) / n;

  map_destruct(&m);
}
