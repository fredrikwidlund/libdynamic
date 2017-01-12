#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

typedef struct element element;
struct element
{
  uint32_t k;
  uint32_t v;
};

typedef struct mapc mapc;
struct mapc
{
  element *elements;
  size_t   elements_count;
  size_t   elements_capacity;
};

static uint64_t ntime()
{
  struct timespec ts;

  (void) clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
  return ((uint64_t) ts.tv_sec * 1000000000) + ((uint64_t) ts.tv_nsec);
}

static size_t mapc_roundup(size_t s)
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

element *mapc_at2(mapc *m, uint32_t k, size_t (*hash)(uint32_t), int (*compare)(uint32_t, uint32_t))
{
  size_t i;
  element *test;

  i = hash(k);
  while (1)
    {
      i &= m->elements_capacity - 1;
      test = &m->elements[i];
      if (compare(test->k, k) == 0 || compare(test->k, (uint32_t) -1) == 0)
        //      if (test->k == k || test->k == (uint32_t) -1)
        //      if (equal(test, element) || equal(test, m->element_empty))
        return test;
      i ++;
    }
}

element *mapc_at(mapc *m, uint32_t k)
{
  size_t i;
  element *test;

  i = k;
  while (1)
    {
      i &= m->elements_capacity - 1;
      test = &m->elements[i];
      if (test->k == k || test->k == (uint32_t) -1)
        return test;
      i ++;
    }
}

static void mapc_reserve(mapc *m, size_t size);

static void mapc_insert(mapc *m, uint32_t k, uint32_t v)
{
  element *test;

  mapc_reserve(m, m->elements_count + 1);
  test = mapc_at(m, k);
  if (test->k == (uint32_t) -1)
    {
      test->k = k;
      test->v = v;
      m->elements_count ++;
    }
}

static void mapc_rehash(mapc *m, size_t size)
{
  mapc new;
  size_t i;

  size = mapc_roundup(size);
  new = *m;
  new.elements_count = 0;
  new.elements_capacity = size;
  new.elements = malloc(new.elements_capacity * sizeof(element));
  for (i = 0; i < new.elements_capacity; i ++)
    new.elements[i] = (element) {(uint32_t) -1, 0};
  if (m->elements)
    {
      for (i = 0; i < m->elements_capacity; i ++)
        if (m->elements[i].k != (uint32_t) -1)
          mapc_insert(&new, m->elements[i].k, m->elements[i].v);
      free(m->elements);
    }
  *m = new;
}

static void mapc_reserve(mapc *m, size_t size)
{
  size *= 2;
  if (size > m->elements_capacity)
    mapc_rehash(m, size);
}

void mapc_construct(mapc *m)
{
  m->elements = NULL;
  m->elements_count = 0;
  m->elements_capacity = 0;
  mapc_rehash(m, 16);
}


void map_custom(uint32_t *a, size_t n, double *insert, double *at)
{
  mapc m;
  element *e;
  size_t i;
  uint64_t t1, t2;

  mapc_construct(&m);

  t1 = ntime();
  for (i = 0; i < n; i ++)
    mapc_insert(&m, a[i], 1);
  t2 = ntime();
  *insert = (double) (t2 - t1) / 1000000000.;

  t1 = ntime();
  for (i = 0; i < n; i ++)
    {
      e = mapc_at(&m, a[i]);
      if (e->v != 1)
        abort();
    }
  t2 = ntime();

  *at = (double) (t2 - t1) / 1000000000.;

  free(m.elements);
}

void map_custom2(uint32_t *a, size_t n, double *insert, double *at, size_t (*hash)(uint32_t), int (*compare)(uint32_t, uint32_t))
{
  mapc m;
  element *e;
  size_t i;
  uint64_t t1, t2;

  mapc_construct(&m);

  t1 = ntime();
  for (i = 0; i < n; i ++)
    mapc_insert(&m, a[i], 1);
  t2 = ntime();
  *insert = (double) (t2 - t1) / 1000000000.;

  t1 = ntime();
  for (i = 0; i < n; i ++)
    {
      //e = mapc_at(&m, a[i]);
      e = mapc_at2(&m, a[i], hash, compare);
      if (e->v != 1)
        abort();
    }
  t2 = ntime();

  *at = (double) (t2 - t1) / 1000000000.;

  free(m.elements);
}
