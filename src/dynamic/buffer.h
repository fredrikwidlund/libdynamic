/*
 * libdynamic buffer.h v0.1.0 - https://github.com/fredrikwidlund/libdynamic
 *
 * Copyright (c) 2016-2024 Fredrik Widlund - fredrik.widlund@gmail.com
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgement in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 */

/*
 * Instructions
 *
 * Include where needed and link with -flto to avoid code duplication
 *
 * Constructors
 *
 * buffer_t  buffer(void)                                                   -- define empty buffer
 *
 * Element access
 *
 * data_t    buffer_data(const buffer_t)                                    -- data
 * uint8_t  *buffer_base(const buffer_t)                                    -- used memory
 * uint8_t  *buffer_end(const buffer_t)                                     -- end of used memory
 * uint8_t  *buffer_memory(const buffer_t)                                  -- allocated memory
 *
 * Capacity
 *
 * size_t    buffer_size(const buffer_t)                                    -- data size
 * int       buffer_empty(const buffer_t)                                   -- buffer empty
 * size_t    buffer_capacity(const buffer_t)                                -- memory size
 *
 * Modifiers
 *
 * void      buffer_reserve(buffer_t *, size_t)                             -- reserve memory
 * void     *buffer_alloc(buffer_t *, size_t, size_t)                       -- allocate data inside buffer
 * void      buffer_resize(buffer_t *, size_t)                              -- resize data
 * void      buffer_insert(buffer_t *, size_t, const data_t)                -- insert
 * void      buffer_prepend(buffer_t *, const data_t)                       -- prepend
 * void      buffer_append(buffer_t *, const data_t)                        -- append
 * void      buffer_erase(buffer_t *, size_t, size_t)                       -- erase data
 * void      buffer_consume(buffer_t *, size_t)                             -- erase from start
 * void      buffer_clear(buffer_t *)                                       -- clear buffer
 *
 */

#ifndef DYNAMIC_BUFFER_H
#define DYNAMIC_BUFFER_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbit.h>
#include <string.h>
#include <sys/uio.h>

#ifndef UNIT_TESTING
#define DYNAMIC_INLINE __attribute__((always_inline)) inline static
#define DYNAMIC_DEF __attribute__((unused, noinline)) static
#else
#define DYNAMIC_INLINE static
#define DYNAMIC_DEF static
#endif
#define dynamic_likely(x) __builtin_expect(!!(x), 1)
#define dynamic_unlikely(x) __builtin_expect(!!(x), 0)

typedef struct iovec data_t;

typedef struct buffer buffer_t;

struct buffer
{
  data_t data;
  data_t memory;
};

/* forward declarations */

DYNAMIC_INLINE uint8_t *buffer_base(const buffer_t);
DYNAMIC_INLINE uint8_t *buffer_memory(const buffer_t);
DYNAMIC_INLINE size_t buffer_size(const buffer_t);
DYNAMIC_INLINE size_t buffer_capacity(const buffer_t);

/* internal */

DYNAMIC_INLINE size_t buffer_offset(buffer_t b)
{
  return buffer_base(b) - buffer_memory(b);
}

DYNAMIC_INLINE void buffer_align(buffer_t *b, size_t pos)
{
  memmove(buffer_memory(*b) + pos, buffer_base(*b), buffer_size(*b));
  b->data.iov_base = buffer_memory(*b) + pos;
}

DYNAMIC_INLINE uint64_t buffer_roundup(uint64_t x)
{
  return x == 0 ? 0 : stdc_bit_ceil(x);
}

/* constructors */

DYNAMIC_INLINE buffer_t buffer(void)
{
  return (buffer_t) {0};
}

/* element access */

DYNAMIC_INLINE data_t buffer_data(const buffer_t b)
{
  return b.data;
}

DYNAMIC_INLINE uint8_t *buffer_base(const buffer_t b)
{
  return b.data.iov_base;
}

DYNAMIC_INLINE uint8_t *buffer_end(const buffer_t b)
{
  return buffer_base(b) + buffer_size(b);
}

DYNAMIC_INLINE uint8_t *buffer_memory(const buffer_t b)
{
  return b.memory.iov_base;
}

/* capacity */

DYNAMIC_INLINE size_t buffer_size(const buffer_t b)
{
  return b.data.iov_len;
}

DYNAMIC_INLINE size_t buffer_empty(const buffer_t b)
{
  return buffer_size(b) == 0;
}

DYNAMIC_INLINE size_t buffer_capacity(const buffer_t b)
{
  return b.memory.iov_len;
}

/* modifiers */

DYNAMIC_INLINE void buffer_reserve(buffer_t *b, size_t capacity)
{
  size_t offset;

  if (dynamic_unlikely(capacity > buffer_capacity(*b)))
  {
    offset = buffer_offset(*b);
    capacity = buffer_roundup(capacity);
    b->memory.iov_base = realloc(buffer_memory(*b), capacity);
    b->memory.iov_len = capacity;
    b->data.iov_base = buffer_memory(*b) + offset;
  }
}

DYNAMIC_INLINE void *buffer_alloc(buffer_t *b, size_t pos, size_t size)
{
  buffer_reserve(b, buffer_size(*b) + size);
  if (pos)
  {
    if (size > buffer_capacity(*b) - buffer_size(*b) - buffer_offset(*b))
      buffer_align(b, 0);
    if (pos < buffer_size(*b))
      memmove(buffer_base(*b) + pos + size, buffer_base(*b) + pos, buffer_size(*b) - pos);
  }
  else if (buffer_size(*b))
  {
    if (size > buffer_offset(*b))
      buffer_align(b, buffer_capacity(*b) - buffer_size(*b));
    b->data.iov_base = buffer_base(*b) - size;
  }
  b->data.iov_len += size;
  return buffer_base(*b) + pos;
}

DYNAMIC_INLINE void buffer_resize(buffer_t *b, size_t size)
{
  if (size > buffer_size(*b))
    (void) buffer_alloc(b, buffer_size(*b), size - buffer_size(*b));
  else if (size == buffer_size(*b))
    b->data.iov_base = buffer_memory(*b);
  b->data.iov_len = size;
}

DYNAMIC_INLINE void buffer_insert(buffer_t *b, size_t pos, const data_t data)
{
  void *base;

  base = buffer_alloc(b, pos, data.iov_len);
  memcpy(base, data.iov_base, data.iov_len);
}

DYNAMIC_INLINE void buffer_prepend(buffer_t *b, const data_t data)
{
  buffer_insert(b, 0, data);
}

DYNAMIC_INLINE void buffer_append(buffer_t *b, const data_t data)
{
  buffer_insert(b, buffer_size(*b), data);
}

DYNAMIC_INLINE void buffer_erase(buffer_t *b, size_t pos, size_t size)
{
  if (pos == 0)
    b->data.iov_base = buffer_base(*b) + size;
  else if (buffer_size(*b) - size - pos)
    memmove(buffer_base(*b) + pos, buffer_base(*b) + pos + size, buffer_size(*b) - size - pos);
  b->data.iov_len -= size;
  if (buffer_empty(*b))
    b->data.iov_base = buffer_memory(*b);
}

DYNAMIC_INLINE void buffer_consume(buffer_t *b, size_t size)
{
  buffer_erase(b, 0, size);
}

DYNAMIC_INLINE void buffer_clear(buffer_t *b)
{
  free(buffer_memory(*b));
  *b = buffer();
}

#endif /* DYNAMIC_BUFFER_H */
