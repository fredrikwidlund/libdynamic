/*
 * libdynamic data.h v0.9.0 - https://github.com/fredrikwidlund/libdynamic
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
 * All non-const data_t arguments below are consumed
 *
 * Constructors
 *
 * data_t   data(void *, size_t)                            -- data from memory
 * data_t   data_null(void)                                 -- empty data
 * data_t   data_string(const char *)                       -- data from string
 * data_t   data_copy(const data_t)                         -- copy data
 * data_t   data_copy_terminate(const data_t)               -- copy with zero byte guard
 * data_t   data_alloc(size_t)                              -- allocate data
 *
 * Element access
 *
 * uint8_t *data_base(const data_t)                         -- data start
 * uint8_t *data_end(const data_t)                          -- data end
 *
 * Capacity
 *
 * size_t   data_size(const data_t)                         -- data size
 * int      data_empty(const data_t)                        -- data empty
 *
 * Modifiers
 *
 * void     data_realloc(data_t *, size_t)                  -- resize data
 * void     data_clear(data_t *)                            -- clear allocated data
 *
 * Operators
 *
 * int      data_equal(const data_t, const data_t)          -- equality
 */

#ifndef DYNAMIC_DATA_H
#define DYNAMIC_DATA_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/uio.h>

#ifndef UNIT_TESTING
#define DYNAMIC_INLINE __attribute__((always_inline)) inline static
#define DYNAMIC_DEF __attribute__((unused, noinline)) static
#else
#define DYNAMIC_INLINE static
#define DYNAMIC_DEF static
#endif

typedef struct iovec data_t;

/* forward declarations */

DYNAMIC_INLINE uint8_t *data_base(const data_t);
DYNAMIC_INLINE size_t data_size(const data_t);

/* constructor/destructor */

DYNAMIC_INLINE data_t data(const void *base, size_t size)
{
  return (data_t) {.iov_base = (void *) base, .iov_len = size};
}

DYNAMIC_INLINE data_t data_string(const char *string)
{
  return data(string, strlen(string));
}

DYNAMIC_INLINE data_t data_null(void)
{
  return data(NULL, 0);
}

DYNAMIC_INLINE data_t data_copy(const data_t d)
{
  size_t size = data_size(d);
  uint8_t *base = malloc(size);

  memcpy(base, data_base(d), size);
  return data(base, size);
}

DYNAMIC_INLINE data_t data_copy_terminate(const data_t d)
{
  size_t size = data_size(d);
  uint8_t *base = malloc(size + 1);

  memcpy(base, data_base(d), size);
  base[size] = 0;
  return data(base, size);
}

DYNAMIC_INLINE data_t data_alloc(size_t size)
{
  return data(malloc(size), size);
}

/* element access */

DYNAMIC_INLINE uint8_t *data_base(const data_t d)
{
  return d.iov_base;
}

DYNAMIC_INLINE uint8_t *data_end(const data_t d)
{
  return data_base(d) + data_size(d);
}

/* capacity */

DYNAMIC_INLINE size_t data_size(const data_t d)
{
  return d.iov_len;
}

DYNAMIC_INLINE size_t data_empty(const data_t d)
{
  return data_size(d) == 0;
}

/* modifiers */

DYNAMIC_INLINE void data_realloc(data_t *d, size_t size)
{
  if (size)
    *d = data(realloc(data_base(*d), size), size);
  else
  {
    free(d->iov_base);
    *d = data_null();
  }
}

DYNAMIC_INLINE void data_clear(data_t *d)
{
  data_realloc(d, 0);
}

/* operators */

DYNAMIC_INLINE int data_equal(const data_t a, const data_t b)
{
  return data_size(a) == data_size(b) && memcmp(data_base(a), data_base(b), data_size(a)) == 0;
}

#endif /* DYNAMIC_DATA_H */
