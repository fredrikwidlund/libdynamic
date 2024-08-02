/*
 * libdynamic map.h v0.9.0 - https://github.com/fredrikwidlund/libdynamic
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
 * All non-const map_t arguments below are consumed
 *
 * Constructors
 *
 * map_t     map(void)                                                          -- define empty map
 *
 * Element access
 *
 * void     *map_lookup(const map_t *, const map_type_t, const void *)          -- find element
 *
 * Capacity
 *
 * size_t    map_size(const map_t)                                              -- count elements
 *
 * Modifiers
 *
 * void      map_rehash(map_t *, const map_type_t, size_t)                      -- resize user memory
 * void      map_clear(map_t *, const map_type_t)                               -- free used memory
 *
 * Iterators
 *
 *           map_foreach(const map_t *, type, iterator)                         -- iterate
 *
 */

#ifndef DYNAMIC_MAP_H
#define DYNAMIC_MAP_H

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

typedef struct map_type map_type_t;
typedef struct map      map_t;

struct map_type
{
  const void * const   null;
  const size_t         size;
  size_t             (*hash)(const void *);
  int                (*empty)(const void *);
  int                (*equal)(const void *, const void *);
  void               (*release)(void *);
};

struct map
{
  void                *base;
  size_t               size;
  size_t               capacity;
};

/* macros */

/* forward declarations */

DYNAMIC_INLINE size_t map_size(const map_t *);

/* internal */

DYNAMIC_INLINE uint64_t map_roundup(uint64_t x)
{
  return x == 0 ? 0 : stdc_bit_ceil(x);
}

/* constructors */

DYNAMIC_INLINE map_t map(const map_type_t type)
{
  return (map_t) {(void *) type.null, 0, .capacity = 1};
}

/* element access */

DYNAMIC_INLINE void *map_at(const map_t *m, const map_type_t type, size_t i)
{
  return (char *) m->base + (i * type.size);
}

DYNAMIC_INLINE void *map_lookup(const map_t *m, const map_type_t type, const void *element)
{
  void *slot;
  size_t i;

  i = type.hash(element);
  while (1)
  {
    i &= m->capacity - 1;
    slot = map_at(m, type, i);
    if (dynamic_likely(type.empty(slot)))
      return slot;
    if (type.equal(element, slot))
      return slot;
    i++;
  }
}

/* capacity */

DYNAMIC_INLINE size_t map_size(const map_t *m)
{
  return m->size;
}

/* modifiers */

DYNAMIC_INLINE void map_rehash(map_t *m, const map_type_t type, size_t size)
{
  void *slot;
  size_t i;
  map_t old = *m;

  m->capacity = map_roundup(size * 2);
  m->base = malloc(m->capacity * type.size);
  for (i = 0; i < m->capacity; i++)
    memcpy(map_at(m, type, i), type.null, type.size);

  for (i = 0; i < old.capacity; i ++)
  {
    slot = map_at(&old, type, i);
    if (!type.empty(slot))
      memcpy(map_lookup(m, type, slot), slot, type.size);
  }
  if (old.base != type.null)
    free(old.base);
}

DYNAMIC_INLINE void map_insert(map_t *m, const map_type_t type, const void *element)
{
  void *slot;

  if (type.empty(element))
    return;

  if (m->size >= m->capacity / 2)
    map_rehash(m, type, m->size + 1);

  slot = map_lookup(m, type, element);
  if (type.empty(slot))
    m->size++;
  else if (type.release)
    type.release(slot);
  memcpy(slot, element, type.size);
}

DYNAMIC_INLINE void map_erase(map_t *m, const map_type_t type, const void *element)
{
  void *slot;
  int i, j, k;

  i = type.hash(element);
  while (1)
  {
    i &= m->capacity - 1;
    slot = map_at(m, type, i);
    if (type.empty(slot))
      return;
    if (type.equal(element, slot))
      break;
    i++;
  }

  if (type.release)
    type.release(slot);
  m->size--;

  j = i;
  while (1)
  {
    j = (j + 1) & (m->capacity - 1);
    if (type.empty(map_at(m, type, j)))
      break;

    k = type.hash(map_at(m, type, j)) & (m->capacity - 1);
    if ((i < j && (k <= i || k > j)) || (i > j && (k <= i && k > j)))
    {
      memcpy(map_at(m, type, i), map_at(m, type, j), type.size);
      i = j;
    }
  }

  memcpy(map_at(m, type, i), type.null, type.size);
}

DYNAMIC_INLINE void map_clear(map_t *m, const map_type_t type)
{
  size_t i;

  if (type.release)
    for (i = 0; i < m->capacity; i++)
      type.release(map_at(m, type, i));
  free(m->base);
  *m = map(type);
}

#endif /* DYNAMIC_MAP_H */
