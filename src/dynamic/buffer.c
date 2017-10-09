#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "buffer.h"

static size_t buffer_roundup(size_t size)
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

static inline void buffer_assert(int value)
{
  if (!value)
    abort();
}

/* constructor/destructor */

void buffer_construct(buffer *b)
{
  *b = (buffer) {0};
}

void buffer_destruct(buffer *b)
{
  buffer_clear(b);
}

/* capacity */

size_t buffer_size(buffer *b)
{
  return b->size;
}

size_t buffer_capacity(buffer *b)
{
  return b->capacity;
}

void buffer_reserve(buffer *b, size_t capacity)
{
  void *data;

  if (capacity > b->capacity)
    {
      capacity = buffer_roundup(capacity);
      data = realloc(b->data, capacity);
      buffer_assert(data != NULL);
      b->data = data;
      b->capacity = capacity;
    }
}

void buffer_compact(buffer *b)
{
  void *data;

  if (b->capacity > b->size)
    {
      data = realloc(b->data, b->size);
      if (b->size)
        buffer_assert(data != NULL);
      b->data = data;
      b->capacity = b->size;
    }
}

/* modifiers */

void buffer_insert(buffer *b, size_t position, void *data, size_t size)
{
  buffer_reserve(b, b->size + size);
  if (position < b->size)
    memmove((char *) b->data + position + size, (char *) b->data + position, b->size - position);
  memcpy((char *) b->data + position, data, size);
  b->size += size;
}

void buffer_insert_fill(buffer *b, size_t position, size_t count, void *data, size_t size)
{
  size_t i;

  buffer_reserve(b, b->size + (count * size));
  if (position < b->size)
    memmove((char *) b->data + position + (count * size), (char *) b->data + position, b->size - position);

  for (i = 0; i < count; i ++)
    memcpy((char *) b->data + position + (i * size), data, size);
  b->size += count * size;
}

void buffer_erase(buffer *b, size_t position, size_t size)
{
  memmove((char *) b->data + position, (char *) b->data + position + size, b->size - position - size);
  b->size -= size;
}

void buffer_clear(buffer *b)
{
  free(b->data);
  buffer_construct(b);
}

/* element access */

void *buffer_data(buffer *b)
{
  return b->data;
}

/* utility */

int buffer_load(buffer *b, char *path)
{
  int fd;
  char block[1048576];
  ssize_t n;

  fd = open(path, O_RDONLY);
  if (fd == -1)
    return -1;

  while (1)
    {
      n = read(fd, block, sizeof block);
      if (n <= 0)
        break;
      buffer_insert(b, buffer_size(b), block, n);
    }
  (void) close(fd);

  return n;
}

int buffer_save(buffer *b, char *path)
{
  int fd;
  char *base;
  size_t size;
  ssize_t n;

  fd = open(path, O_CREAT | O_WRONLY, 0644);
  if (fd == -1)
    return -1;

  base = buffer_data(b);
  size = buffer_size(b);
  while (size)
    {
      n = write(fd, base, size);
      if (n == -1)
        break;
      base += n;
      size -= n;
    }
  (void) close(fd);

  return size ? -1 : 0;
}
