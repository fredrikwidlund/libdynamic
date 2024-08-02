/*
 * libdynamic list.h v0.9.0 - https://github.com/fredrikwidlund/libdynamic
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
 * All non-const list_t arguments below are consumed
 *
 * Constructors
 *
 * list_t  list(list_t *)                                        -- define empty list
 * list_t *list_create(void)                                     -- allocate list
 * void    list_destroy(list_t *)                                -- free list
 *
 * Element access
 *
 * void   *list_front(const list_t *)                            -- first element
 * void   *list_back(const list_t *)                             -- last element
 * void   *list_end(const list_t *)                              -- last element
 *
 * Capacity
 *
 * size_t  list_size(const list_t *)                             -- number of elements
 * int     list_empty(const list_t *)                            -- empty list
 *
 * Modifiers
 *
 * void   *list_alloc(void *, size_t)                            -- alloc before iter
 * void    list_insert(void *, const void *, size_t)             -- insert before iter
 * void    list_append(list_t *, const void *, size_t)           -- append
 * void    list_prepend(list_t *, const void *, size_t)          -- prepend
 * void    list_splice(void *, void *)                           -- move element
 * void    list_clear(list_t *)                                  -- clear list
 *
 * Iterators
 *
 *         list_foreach(list_t *, void *)                        -- loop
 *         list_foreach_reverse(list *, void *)                  -- loop reverse
 * void   *list_prev(const void *)                               -- prev element
 * void   *list_next(const void *)                               -- next element
 *
 */

#ifndef DYNAMIC_LIST_H
#define DYNAMIC_LIST_H

#include <stdint.h>
#include <stddef.h>
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
#define dynamic_likely(x) __builtin_expect(!!(x), 1)
#define dynamic_unlikely(x) __builtin_expect(!!(x), 0)

typedef struct list list_t;

struct list
{
  list_t *next;
  list_t *prev;
  char element[];
};

/* macros */

#define list_foreach(l, e) for (e = list_front(l); e != list_end(l); e = list_next(e))
#define list_foreach_reverse(l, e) for (e = list_back(l); e != list_end(l); e = list_prev(e))

/* forward declarations */

DYNAMIC_INLINE void *list_prev(const void *);
DYNAMIC_INLINE void *list_next(const void *);
DYNAMIC_INLINE void *list_end(list_t *);
DYNAMIC_INLINE void list_clear(list_t *);

/* internal */

DYNAMIC_INLINE list_t *list_node(const void *element)
{
  return (list_t *) ((uintptr_t) element - offsetof(list_t, element));
}

/* constructors */

DYNAMIC_INLINE list_t list(list_t *l)
{
  *l = (list_t) {.next = l, .prev = l};
  return *l;
}

DYNAMIC_INLINE list_t *list_create(void)
{
  list_t *l = malloc(sizeof *l);
  list(l);
  return l;
}

DYNAMIC_INLINE void list_destroy(list_t *l)
{
  list_clear(l);
  free(l);
}

/* element access */

DYNAMIC_INLINE void *list_front(const list_t *l)
{
  return l->next->element;
}

DYNAMIC_INLINE void *list_back(const list_t *l)
{
  return l->prev->element;
}

DYNAMIC_INLINE void *list_end(list_t *l)
{
  return l->element;
}

/* capacity */

DYNAMIC_INLINE int list_empty(const list_t *l)
{
  return l == l->prev;
}

/* modifiers */

DYNAMIC_INLINE void *list_alloc(void *iter, size_t size)
{
  list_t *node, *next;

  next = list_node(iter);
  node = malloc(sizeof (list_t) + size);
  node->prev = next->prev;
  node->next = next;
  next->prev = node;
  node->prev->next = node;
  return node->element;
}

DYNAMIC_INLINE void list_insert(void *iter, const void *base, size_t size)
{
  memcpy(list_alloc(iter, size), base, size);
}

DYNAMIC_INLINE void list_prepend(list_t *l, const void *base, size_t size)
{
  list_insert(list_front(l), base, size);
}

DYNAMIC_INLINE void list_append(list_t *l, const void *base, size_t size)
{
  list_insert(l->element, base, size);
}

DYNAMIC_INLINE void list_splice(void *iter, void *element)
{
  list_t *node, *next;

  if (dynamic_unlikely(iter == element))
    return;
  node = list_node(element);
  node->prev->next = node->next;
  node->next->prev = node->prev;

  next = list_node(iter);
  node->prev = next->prev;
  node->next = next;
  next->prev = node;
  node->prev->next = node;
}

DYNAMIC_INLINE void list_erase(void *element)
{
  list_t *node;

  node = list_node(element);
  node->prev->next = node->next;
  node->next->prev = node->prev;
  free(node);
}

DYNAMIC_INLINE void list_clear(list_t *l)
{
  list_t *node, *next;

  node = l->next;
  while (node != l)
  {
    next = node->next;
    free(node);
    node = next;
  }
  *l = list(l);
}

/* iterators */

DYNAMIC_INLINE void *list_prev(const void *element)
{
  return list_node(element)->prev->element;
}

DYNAMIC_INLINE void *list_next(const void *element)
{
  return list_node(element)->next->element;
}

#endif /* DYNAMIC_LIST_H */
