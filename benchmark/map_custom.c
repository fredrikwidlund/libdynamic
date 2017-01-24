#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/param.h>
#include <time.h>
#include <err.h>

#include "map.h"

typedef struct map_element map_element;
struct map_element
{
  int key;
  int value;
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

static map_element *map_at(map *m, int key)
{
  size_t i;
  map_element *e;

  i = key;
  while (1)
    {
      i &= m->elements_capacity - 1;
      e = &m->elements[i];
      if (e->key == key || e->key == -1)
        return e;
      i ++;
    }
}

static void map_insert(map *m, int key, int value)
{
  map_element *e;

  map_reserve(m, m->elements_count + 1);
  e = map_at(m, key);
  if (e->key == -1)
    {
      e->key = key;
      e->value = value;
      m->elements_count ++;
    }
}

static void map_erase(map *m, int key)
{
  map_element *e;
  size_t i, j, k;

  e = map_at(m, key);
  if (e->key == -1)
    return;

  m->elements_count --;

  i = ((char *) e - (char *) m->elements) / sizeof *e;
  j = i;
  while (1)
    {
      j = (j + 1) & (m->elements_capacity - 1);
      if (m->elements[j].key == -1)
        break;

      k = m->elements[j].key & (m->elements_capacity - 1);
      if ((i < j && (k <= i || k > j)) ||
          (i > j && (k <= i && k > j)))
        {
          m->elements[i].key = m->elements[j].key;
          m->elements[i].value = m->elements[j].value;
          i = j;
        }
    }

  m->elements[i].key = -1;
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
    new.elements[i] = (map_element) {-1, 0};
  if (m->elements)
    {
      for (i = 0; i < m->elements_capacity; i ++)
        if (m->elements[i].key != -1)
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

void map_custom(int *keys, int *keys_shuffled, int *values, size_t size, size_t lookups, double *insert, double *lookup, double *erase, uint64_t *sum)
{
  map m;
  uint64_t t1, t2, s;
  size_t i, n;

  map_construct(&m);

  t1 = ntime();
  for (i = 0; i < size; i ++)
    map_insert(&m, keys[i], values[i]);
  t2 = ntime();
  *insert = (double) (t2 - t1) / size;

  s = 0;
  t1 = ntime();
  for (n = lookups; n; n -= i)
    for (i = 0; i < MIN(size, n); i ++)
      s += map_at(&m, keys_shuffled[i])->value;
  t2 = ntime();
  *sum = s;
  *lookup = (double) (t2 - t1) / lookups;

  t1 = ntime();
  for (i = 0; i < size; i ++)
    map_erase(&m, keys[i]);
  t2 = ntime();
  if (m.elements_count)
    errx(1, "inconsistency");
  *erase = (double) (t2 - t1) / size;

  map_destruct(&m);
}
