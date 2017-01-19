#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "buffer.h"
#include "vector.h"
#include "map.h"
#include "map_int.h"

static size_t hash(map *m, void *element)
{
  (void) m;
  return *(int *) element;
}

static int equal(map *m, void *a, void *b)
{
  (void) m;
  return *(int *) a == *(int *) b;
}

static void set(map *m, void *dst, void *src)
{
  (void) m;
  memcpy(dst, src, m->element_size);
}

/* constructor/destructor */

void map_int_construct(map_int *m, size_t element_size)
{
  m->empty = malloc(element_size);
  *(int *) m->empty = -1;
  m->release = NULL;
  map_construct(&m->map, element_size, &(m->empty), set);
}

void map_int_destruct(map_int *m)              {map_destruct(&m->map, equal, m->release); free(m->empty);}

/* capacity */

size_t map_int_size(map_int *m)                {return map_size(&m->map);}
void map_int_reserve(map_int *m, size_t size)  {map_reserve(&m->map, size, hash, equal, set);}

/* element access */

int map_int_element_empty(map_int *m)          {return *(int *) m->empty;}
void *map_int_at(map_int *m, int key)          {return map_at(&m->map, &key, hash, equal);}

/* modifiers */

void map_int_insert(map_int *m, void *element) {map_insert(&m->map, element, hash, equal, set, m->release);}
void map_int_erase(map_int *m, int key)        {map_erase(&m->map, &key, hash, equal, set, m->release);}
void map_int_clear(map_int *m)                 {map_clear(&m->map, equal, set, m->release);}

