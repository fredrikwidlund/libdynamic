#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buffer.h"
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
  buffer_init(&v->data);
  v->object_size = object_size;
  v->release = NULL;
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
  return buffer_size(&v->data) / v->object_size;
}

/*
int vector_resize(vector *v, size_t size)
{
  int e;
  
  e = vector_reserve(v, size);
  if (e == -1)
    return -1;

  v->size = size;
  return 0;
}
*/

size_t vector_capacity(vector *v)
{
  return buffer_capacity(&v->data) / v->object_size;
}

int vector_empty(vector *v)
{
  return vector_size(v) == 0;
}

int vector_reserve(vector *v, size_t capacity)
{
  return buffer_reserve(&v->data, capacity * v->object_size);
}

int vector_shrink_to_fit(vector *v)
{
  return buffer_compact(&v->data);
}

/* element access */

void *vector_at(vector *v, size_t index)
{  
  return buffer_data(&v->data) + (index * v->object_size);
}

void *vector_front(vector *v)
{
  return vector_at(v, 0);
}

void *vector_back(vector *v)
{
  return buffer_end(&v->data) - v->object_size;
}

void *vector_data(vector *v)
{
  return buffer_data(&v->data);
}

/* modifiers */

int vector_push_back(vector *v, void *value)
{
  return buffer_append(&v->data, value, v->object_size);
}

/*
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
*/

void vector_clear(vector *v)
{
  buffer_clear(&v->data);
}
