/*
 * libdynamic vector.h v0.9.0 - https://github.com/fredrikwidlund/libdynamic
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
 * All non-const vector_t arguments below are consumed
 *
 * Constructors
 *
 * vector_t  vector(void)                                   -- define empty vector
 * vector_t  vector_data(const void *, size_t)              -- define from memory
 * vector_t  vector_literal(type, elements...)              -- define constant vector
 *
 * Element access
 *
 * void     *vector_base(const vector_t)                    -- vector pointer
 * type      vector_at(const vector_t, type, size_t)        -- vector member
 *
 * Capacity
 *
 * size_t    vector_size(const vector_t)                    -- vector size size
 * int       vector_empty(const vector_t)                   -- vector empty
 * size_t    vector_length(const vector_t, type)            -- count elemets
 *
 * Modifiers
 *
 * void      vector_resize(vector_t *, size_t)              -- resize user memory
 * void      vector_push(vector_t *, type, elements...)     -- push elements
 * void      vector_clear(vector_t *)                       -- free used memory
 *
 * Iterators
 *
 *           vector_foreach(const vector_t, type, iterator) -- iterate
 *
 */

#ifndef DYNAMIC_VECTOR_H
#define DYNAMIC_VECTOR_H

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

typedef struct iovec vector_t;

/* macros */

#define vector_literal(type, ...)  vector_data((type[]){__VA_ARGS__}, sizeof ((type[]){__VA_ARGS__}))
#define vector_length(v, type)     (vector_size(v) / sizeof(type))
#define vector_at(v, type, i)      (((type *) vector_base(v))[i])
#define vector_foreach(v, type, e) for (size_t __vector_iter = 0;       \
                                        __vector_iter < vector_length(v, type) && \
                                          ((e = vector_at(v, type,__vector_iter)) || 1); \
                                        __vector_iter++)
#define vector_push(v, type, ...)  vector_push_vector(v, vector_literal(type, __VA_ARGS__));
#define vector_pop(v, type)        vector_resize(v, vector_size(*v) - sizeof (type))

/* forward declarations */

DYNAMIC_INLINE void *vector_base(const vector_t);
DYNAMIC_INLINE size_t vector_size(const vector_t);

/* internal */

DYNAMIC_INLINE uint64_t vector_roundup(uint64_t x)
{
  return x == 0 ? 0 : stdc_bit_ceil(x);
}

/* constructors */

DYNAMIC_INLINE vector_t vector_data(const void *base, size_t size)
{
  return (vector_t) {.iov_base = (void *) base, .iov_len = size};
}

DYNAMIC_INLINE vector_t vector(void)
{
  return vector_data(NULL, 0);
}

/* element access */

DYNAMIC_INLINE void *vector_base(const vector_t v)
{
  return v.iov_base;
}

/* capacity */

DYNAMIC_INLINE size_t vector_size(const vector_t v)
{
  return v.iov_len;
}

DYNAMIC_INLINE size_t vector_empty(const vector_t v)
{
  return vector_size(v) == 0;
}

/* modifiers */

DYNAMIC_INLINE void vector_resize(vector_t *v, size_t size)
{
  size_t capacity;

  if (size)
  {
    capacity = vector_roundup(size);
    if (dynamic_unlikely(capacity != vector_roundup(vector_size(*v))))
      v->iov_base = realloc(vector_base(*v), capacity);
    v->iov_len = size;
  }
  else
  {
    free(vector_base(*v));
    *v = vector();
  }
}

DYNAMIC_INLINE void vector_push_vector(vector_t *v, const vector_t args)
{
  size_t size = vector_size(*v);

  vector_resize(v, size + vector_size(args));
  memcpy((char *) vector_base(*v) + size, vector_base(args), vector_size(args));
}

DYNAMIC_INLINE void vector_clear(vector_t *v)
{
  vector_resize(v, 0);
}

#endif /* DYNAMIC_VECTOR_H */
