#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vector.h"

/* allocators */

vector *vector_new(size_t object_size)
{
  vector *v;
  
  v = malloc(sizeof *v);
  if (!v)
    return NULL;
    
  vector_init(v, object_size);

  return v;
}

void vector_free(vector *v)
{
  (void) vector_clear(v);
  free(v);
}

void vector_init(vector *v, size_t object_size)
{
  memset(v, 0, sizeof *v);
  v->object_size = object_size;
}

void vector_release(vector *v, void (*release)(void *))
{
  v->release = release;
}

void *vector_deconstruct(vector *v)
{
  int e;
  void *data;

  e = vector_shrink_to_fit(v);
  if (e == -1)
    return NULL;
  
  data = vector_data(v);
  free(v);

  return data;
}

/* capacity */

size_t vector_size(vector *v)
{
  return v->size;
}

int vector_resize(vector *v, size_t size)
{
  int e;
  
  e = vector_reserve(v, size);
  if (e == -1)
    return -1;

  v->size = size;
  return 0;
}

size_t vector_capacity(vector *v)
{
  return v->capacity;
}

int vector_empty(vector *v)
{
  return vector_size(v) == 0;
}

int vector_reserve(vector *v, size_t capacity)
{
  if (capacity > v->capacity)
    {
      size_t size = vector_roundup_size(capacity * v->object_size);
      void *data = realloc(v->data, size);

      if (!data)
        return -1;

      v->capacity = size / v->object_size;
      v->data = data;
    }

  return 0;
}

int vector_shrink_to_fit(vector *v)
{
  if (v->capacity > v->size)
    {
      void *data = realloc(v->data, v->size * v->object_size);
      
      if (v->size > 0 && !data)
        return -1;

      v->capacity = v->size;
      v->data = data;
    }

  return 0;
}

/* element access */

void *vector_at(vector *v, size_t index)
{  
  return ((char *) v->data) + (index * v->object_size);
}

void *vector_front(vector *v)
{
  return vector_at(v, 0);;
}

void *vector_back(vector *v)
{
  return vector_at(v, v->size - 1);
}

void *vector_data(vector *v)
{
  return v->data;
}

/* modifiers */

int vector_push_back(vector *v, void *value)
{
  return vector_insert(v, v->size, 1, value);
}

void vector_pop_back(vector *v)
{
  vector_erase(v, v->size - 1, v->size);
}

int vector_insert(vector *v, size_t position, size_t size, void *base)
{
  int e;

  e = vector_resize(v, v->size + size);
  if (e == -1)
    return -1;

  memmove(vector_at(v, position + size), vector_at(v, position),
          (v->size - size - position) * v->object_size);
  memcpy(vector_at(v, position), base, size * v->object_size);

  return 0;
}

void vector_erase(vector *v, size_t from, size_t to)
{
  size_t i;

  if (v->release)
    for (i = from; i < to; i ++)
      v->release(vector_at(v, i));
  
  memmove(vector_at(v, from), vector_at(v, to), (v->size - to) * v->object_size);
  v->size -= to - from;
}

int vector_clear(vector *v)
{
  vector_erase(v, 0, vector_size(v));
  return vector_shrink_to_fit(v);
}

/* internals */

size_t vector_roundup_size(size_t size)
{
  size --;
  size |= size >> 1;
  size |= size >> 2;
  size |= size >> 4;
  size |= size >> 8;
  size |= size >> 16;
  size |= size >> 32;
  size ++;

  return size;
}
