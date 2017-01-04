#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "buffer.h"
#include "vector.h"

/* constructor/destructor */

void vector_construct(vector *v, size_t object_size)
{
  buffer_construct(&v->buffer);
  v->object_size = object_size;
  v->object_release = NULL;
  v->object_copy= NULL;
}

void vector_object_release(vector *v, void (*release)(void *))
{
  v->object_release = release;
}

void vector_object_copy(vector *v, void (*copy)(void *, void *))
{
  v->object_copy = copy;
}

void vector_destruct(vector *v)
{
  vector_clear(v);
}

void vector_copy(vector *v, vector *source)
{
  size_t i;

  vector_construct(v, source->object_size);
  vector_object_release(v, source->object_release);
  vector_object_copy(v, source->object_copy);
  vector_insert(v, 0, vector_size(source), vector_data(source));
  if (v->object_copy)
    for (i = 0; i < vector_size(v); i ++)
      v->object_copy(vector_at(v, i), vector_at(source, i));
}

/* capacity */

size_t vector_size(vector *v)
{
  return buffer_size(&v->buffer) / v->object_size;
}

size_t vector_capacity(vector *v)
{
  return buffer_capacity(&v->buffer) / v->object_size;
}

int vector_empty(vector *v)
{
  return vector_size(v) == 0;
}

void vector_reserve(vector *v, size_t capacity)
{
  buffer_reserve(&v->buffer, capacity * v->object_size);
}

void vector_shrink_to_fit(vector *v)
{
  buffer_compact(&v->buffer);
}

/* element access */

void *vector_at(vector *v, size_t position)
{
  return (char *) buffer_data(&v->buffer) + (position * v->object_size);
}

void *vector_front(vector *v)
{
  return vector_data(v);
}

void *vector_back(vector *v)
{
  return (char *) buffer_data(&v->buffer) + buffer_size(&v->buffer) - v->object_size;
}

void *vector_data(vector *v)
{
  return buffer_data(&v->buffer);
}

/* modifiers */

void vector_push_back(vector *v, void *object)
{
  buffer_insert(&v->buffer, v->buffer.size, object, v->object_size);
}

void vector_pop_back(vector *v)
{
  size_t size = vector_size(v);

  vector_erase(v, size - 1, size);
}

void vector_insert(vector *v, size_t position, size_t size, void *object)
{
  buffer_insert(&v->buffer, position * v->object_size, object, size * v->object_size);
}

void vector_erase(vector *v, size_t from, size_t to)
{
  size_t i;

  if (v->object_release)
    for (i = from; i < to; i ++)
      v->object_release(vector_at(v, i));

  buffer_erase(&v->buffer, from * v->object_size, (to - from) * v->object_size);
}

void vector_clear(vector *v)
{
  vector_erase(v, 0, vector_size(v));
  buffer_clear(&v->buffer);
}
