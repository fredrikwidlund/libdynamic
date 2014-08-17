#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

#include "buffer.h"

/* allocators */

buffer *buffer_new()
{
  buffer *b;
  
  b = malloc(sizeof *b);
  if (!b)
    return NULL;
  
  buffer_init(b);
  
  return b;
}

void buffer_free(buffer *b)
{
  buffer_clear(b);
  free(b);
}

void buffer_init(buffer *b)
{
  b->base = NULL;
  b->top = NULL;
  b->memory_base = NULL;
  b->memory_top = NULL;
}

char *buffer_deconstruct(buffer *b)
{
  char *data;
  
  buffer_compact(b);
  data = buffer_data(b);
  free(b);

  return data;
}

/* capacity */

size_t buffer_size(buffer *b)
{
  return b->top - b->base;
}

size_t buffer_capacity(buffer *b)
{
  return b->memory_top - b->memory_base;
}

size_t buffer_capacity_head(buffer *b)
{
  return b->base - b->memory_base;
}

size_t buffer_capacity_tail(buffer *b)
{
  return b->memory_top - b->top;
}

int buffer_reserve(buffer *b, size_t capacity)
{
  char *memory_base;

  if (capacity > buffer_capacity(b))
    {
      capacity = buffer_roundup(capacity);
      memory_base = realloc(b->memory_base, capacity);
      if (!memory_base)
        return -1;

      b->base += memory_base - b->memory_base;
      b->top += memory_base - b->memory_base;
      b->memory_base = memory_base;
      b->memory_top = memory_base + capacity;
    }

  return 0;
}

void buffer_move(buffer *b, size_t size)
{
  printf("[move %ld]\n", size);
  memmove(b->base + size, b->base, buffer_size(b));
  b->base += size;
  b->top += size;
}

int buffer_compact(buffer *b)
{
  printf("[buffer compact]\n");
  
  return 0;
}

/* modifiers */

int buffer_prepend(buffer *b, char *data, size_t size)
{
  int e;

  e = buffer_reserve(b, buffer_size(b) + size);
  if (e == -1)
    return -1;

  if (buffer_capacity_head(b) < size)
    buffer_move(b, size - buffer_capacity_head(b));
  b->base -= size;
  memcpy(b->base, data, size);

  return 0;
}

int buffer_append(buffer *b, char *data, size_t size)
{
  int e;

  e = buffer_reserve(b, buffer_size(b) + size);
  if (e == -1)
    return -1;

  if (buffer_capacity_tail(b) < size)
    buffer_move(b, - (size - buffer_capacity_head(b)));
  memcpy(b->top, data, size);
  b->top += size;
  
  return 0;
}

void buffer_insert(buffer *b, size_t position, char *data, size_t size)
{
  printf("[buffer insert]\n");
}


void buffer_clear(buffer *b)
{
  free(b->memory_base);
  buffer_init(b);
}

/* element access */

char *buffer_data(buffer *b)
{
  return b->base;
}

char *buffer_end(buffer *b)
{
  return b->top;
}

/* internals */

size_t buffer_roundup(size_t size)
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
