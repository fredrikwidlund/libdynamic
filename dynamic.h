/*
 * libdynamic v0.9.1 - https://github.com/fredrikwidlund/libdynamic
 *
 * Copyright (c) 2016-2025 Fredrik Widlund - fredrik.widlund@gmail.com
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

#ifndef DYNAMIC_H
#define DYNAMIC_H

#include <stdio.h>
#include <stdbit.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>
#include <sys/uio.h>

#ifndef UNIT_TESTING
#define INLINE   __attribute__((always_inline)) inline static
#define NOINLINE __attribute__((unused, noinline)) static
#else
#define INLINE static
#define NOINLINE static
#endif /* UNIT_TESTING */

#define DYNAMIC_VERSION       "0.9.1"
#define DYNAMIC_VERSION_MAJOR 0
#define DYNAMIC_VERSION_MINOR 9
#define DYNAMIC_VERSION_PATCH 1

#define dynamic_likely(x)   __builtin_expect(!!(x), 1)
#define dynamic_unlikely(x) __builtin_expect(!!(x), 0)

/* TYPES */

typedef struct iovec     data_t;
typedef data_t           vector_t;
typedef data_t           string_t;
typedef struct list_node list_node_t;
typedef void             list_iter_t;
typedef struct list      list_t;
typedef struct buffer    buffer_t;
typedef buffer_t         pool_t;

struct list
{
  list_iter_t *front;
  size_t       length;
};

struct list_node
{
  list_node_t *next;
  list_node_t *prev;
  uint8_t      element[];
};

struct buffer
{
  data_t data;
  data_t memory;
};

/* DATA */

#define             data_concat(...)                 data_concat_vector(vector(data_t, __VA_ARGS__))

INLINE uint64_t     data_roundup(uint64_t);
INLINE data_t       data(const void *, size_t);
INLINE data_t       data_string(const char *);
INLINE uint8_t     *data_base(const data_t);
INLINE size_t       data_size(const data_t);
INLINE data_t       data_null(void);
INLINE bool         data_nullp(const data_t);
INLINE bool         data_emptyp(const data_t);
INLINE bool         data_equalp(const data_t, const data_t);
INLINE data_t       data_find(const data_t, const data_t);
INLINE size_t       data_offset(const data_t, const data_t);
INLINE data_t       data_range(const data_t , size_t, size_t);
INLINE data_t       data_resize(const data_t, size_t);
INLINE data_t       data_consume(const data_t, size_t);
INLINE data_t       data_shift(const data_t, size_t);
INLINE data_t       data_insert(const data_t, size_t, const data_t);
INLINE data_t       data_erase(data_t, size_t, size_t);
INLINE data_t       data_append(const data_t, const data_t);
INLINE data_t       data_alloc(size_t);
INLINE data_t       data_realloc(data_t , size_t);
INLINE data_t       data_free(data_t);

/* STRING */

#define             string_concat(...)               string_concat_vector(vector(string_t, __VA_ARGS__))
#define             string_foreach(s, c)             for (string_t __iter = s; \
                                                          c = string_first(__iter), !string_emptyp(c); \
                                                          __iter = string_rest(__iter, c), \
                                                            c = string_first(__iter))

INLINE string_t     string(const char *);
INLINE uint8_t     *string_base(const string_t);
INLINE size_t       string_size(const string_t);
INLINE size_t       string_length(const string_t);
INLINE string_t     string_null(void);
INLINE bool         string_nullp(const string_t );
INLINE bool         string_emptyp(const string_t );
INLINE bool         string_equalp(const string_t, const string_t);
INLINE string_t     string_find(const string_t, const string_t);
INLINE string_t     string_character(data_t, uint32_t);
INLINE uint32_t     string_codepoint(const string_t);
INLINE string_t     string_first(const string_t);
INLINE string_t     string_rest(const string_t, const string_t);
INLINE bool         string_valid(const string_t);
INLINE size_t       string_fwrite(const string_t, FILE *);
/* below returns a value that needs to be released with string_free() */
INLINE string_t     string_copy(const string_t);
NOINLINE string_t   string_concat_vector(const vector_t);
NOINLINE string_t   string_replace(string_t, const string_t, const string_t);
/* below also consumes/reuses the first value which is no longer valid after the call */
INLINE string_t     string_insert(string_t, size_t, const string_t);
INLINE string_t     string_erase(string_t, size_t, size_t);
INLINE string_t     string_prepend(string_t, const string_t);
INLINE string_t     string_append(string_t, const string_t);
/* release allocated memory */
INLINE string_t     string_free(string_t);

/* VECTOR */

#define             __vector_args(type, ...)         data((type[]){__VA_ARGS__}, sizeof ((type[]){__VA_ARGS__}))
#define             __vector(void)                   data_null();
#define             vector(...)                      __vector##__VA_OPT__(_args)(__VA_ARGS__)
#define             vector_length(v, type)           (data_size(v) / sizeof (type))
#define             vector_at(v, type, pos)          ((type *) data_base(v))[pos]
#define             vector_front(v, type)            vector_at(v, type, 0)
#define             vector_back(v, type)             vector_at(v, type, vector_length(v, type) - 1)
#define             vector_insert(v, type, pos, ...) vector_insert_data(v, sizeof (type) * pos, vector(type, __VA_ARGS__))
#define             vector_erase(v, type, from, to)  vector_erase_data(v, data_range(v, from * sizeof (type), to * sizeof (type)))
#define             vector_push_back(v, type, ...)   vector_insert_data(v, vector_size(v), vector(type, __VA_ARGS__))
#define             vector_pop_back(v, type)         vector_erase_data(v, data_consume(v, vector_size(v) - sizeof (type)));
#define             vector_foreach(v, type, e)       for (size_t __iter = 0; \
                                                          __iter < vector_length(v, type) && \
                                                            (e = vector_at(v, type, __iter), 1); \
                                                          __iter++)

INLINE void        *vector_base(const vector_t);
INLINE size_t       vector_size(const vector_t);
INLINE bool         vector_emptyp(const vector_t);
INLINE bool         vector_equalp(const vector_t, const vector_t);
INLINE vector_t     vector_copy(const vector_t);
/* below consumes/reuses the first value which is no longer valid after the call */
INLINE vector_t     vector_insert_data(vector_t, size_t, const data_t);
INLINE vector_t     vector_erase_data(vector_t, const data_t);
INLINE vector_t     vector_free(vector_t);

/* LIST */

/* below variables are l(ist), i(terator), t(ype), and e(lement) */
#define             __list()                         list_null()
#define             __list_args(t, ...)              list_insert(__list(), NULL, t, __VA_ARGS__)
#define             __list_insert(l, i, t, e)        list_insert_data(l, i, vector(t, e))
#define             __list_insert_args(l, i, t, ...) list_insert_vector(l, i, sizeof (t), vector(t, __VA_ARGS__))
#define             list(...)                        __list##__VA_OPT__(_args)(__VA_ARGS__)
#define             list_insert(l, i, t, e, ...)     __list_insert##__VA_OPT__(_args)(l, i, t, e __VA_OPT__(,) __VA_ARGS__)
#define             list_push_front(l, t, ...)        list_insert(l, NULL, t, __VA_ARGS__)
#define             list_push_back(l, t, ...)        list_insert(l, list_back(l), t, __VA_ARGS__)
#define             list_remove(l, t, e)             list_remove_data(l, vector(t, e))
#define             list_foreach(l, t, e)            for (t *__iter = list_front(l); \
                                                          __iter && (e = *__iter, 1); \
                                                          __iter = __iter == list_back(l) ? NULL : list_next(__iter))

INLINE list_t       list_null(void);
INLINE size_t       list_length(const list_t);
INLINE bool         list_emptyp(const list_t);
INLINE list_node_t *list_node(const list_iter_t *);
INLINE list_iter_t *list_next(const list_iter_t *);
INLINE list_iter_t *list_prev(const list_iter_t *);
INLINE list_iter_t *list_front(const list_t);
INLINE list_iter_t *list_back(const list_t);
INLINE list_t       list_insert_node(list_t , list_iter_t *, list_node_t *);
INLINE list_t       list_insert_alloc(list_t, list_iter_t *, size_t, void **);
INLINE list_t       list_insert_data(list_t, list_iter_t *, data_t);
INLINE list_t       list_insert_vector(list_t, list_iter_t *, size_t, vector_t);
INLINE list_t       list_remove_node(list_t, list_node_t *);
INLINE list_t       list_remove_data(list_t, data_t);
INLINE list_t       list_remove_if(list_t, bool (*)(list_iter_t *));
INLINE list_t       list_erase(list_t, list_iter_t *);
INLINE list_t       list_pop_back(list_t);
INLINE list_t       list_pop_front(list_t);
INLINE list_t       list_splice(list_t, list_iter_t *, list_t *, list_iter_t *);
INLINE list_t       list_free(list_t);

/* BUFFER */

INLINE buffer_t     buffer(void);
INLINE data_t       buffer_data(const buffer_t);
INLINE data_t       buffer_memory(const buffer_t);
INLINE size_t       buffer_size(const buffer_t);
INLINE bool         buffer_emptyp(const buffer_t);
INLINE size_t       buffer_capacity(const buffer_t);
INLINE buffer_t     buffer_align(buffer_t, size_t);
INLINE buffer_t     buffer_alloc(buffer_t , size_t, size_t, void **);
INLINE buffer_t     buffer_insert(buffer_t, size_t, const data_t);
INLINE buffer_t     buffer_prepend(buffer_t, const data_t);
INLINE buffer_t     buffer_append(buffer_t, const data_t);
INLINE buffer_t     buffer_erase(buffer_t, size_t, size_t);
INLINE buffer_t     buffer_clear(buffer_t);
INLINE buffer_t     buffer_free(buffer_t);

/* POOL */

INLINE pool_t       pool(void);
INLINE pool_t       pool_alloc(pool_t, void **, size_t);
INLINE pool_t       pool_return(pool_t, void *);
INLINE pool_t       pool_free(pool_t);

/* IMPLEMENTATION */

/* DATA */

INLINE uint64_t data_roundup(uint64_t v)
{
  return v == 0 ? 0 : stdc_bit_ceil(v);
}

INLINE data_t data(const void *base, size_t size)
{
  return (data_t) {.iov_base = (void *) base, .iov_len = size};
}

INLINE uint8_t *data_base(const data_t d)
{
  return d.iov_base;
}

INLINE size_t data_size(const data_t d)
{
  return d.iov_len;
}

INLINE data_t data_null(void)
{
  return data(NULL, 0);
}

INLINE data_t data_string(const char *chars)
{
  return data(chars, strlen(chars));
}

INLINE bool data_nullp(const data_t d)
{
  return data_base(d) == NULL;
}

INLINE bool data_emptyp(const data_t d)
{
  return data_size(d) == 0;
}

INLINE bool data_equalp(const data_t d1, const data_t d2)
{
  return data_size(d1) == data_size(d2) && memcmp(data_base(d1), data_base(d2), data_size(d1)) == 0;
}

INLINE data_t data_find(const data_t d1, const data_t d2)
{
  void *p;

  p = memmem(data_base(d1), data_size(d1), data_base(d2), data_size(d2));
  return p ? data(p, data_size(d2)) : data_consume(d1, data_size(d1));
}

INLINE size_t data_offset(const data_t d1, const data_t d2)
{
  return data_base(d2) - data_base(d1);
}

INLINE data_t data_range(const data_t d, size_t from, size_t to)
{
  return data(data_base(d) + from, to - from);
}

INLINE data_t data_resize(const data_t d, size_t size)
{
  return data_range(d, 0, size);
}

INLINE data_t data_consume(const data_t d, size_t size)
{
  return data_range(d, size, data_size(d));
}

INLINE data_t data_shift(const data_t d, size_t offset)
{
  return data_range(d, offset, data_size(d) + offset);
}

INLINE data_t data_move(data_t to, data_t from)
{
  (void) memmove(data_base(to), data_base(from), data_size(from));
  return data_resize(to, data_size(from));
}

INLINE data_t data_copy(data_t to, data_t from)
{
  (void) memcpy(data_base(to), data_base(from), data_size(from));
  return data_resize(to, data_size(from));
}

INLINE data_t data_insert(data_t d1, size_t pos, const data_t d2)
{
  if (pos < data_size(d1))
  {
    (void) data_move(
      data_shift(data_consume(d1, pos), data_size(d2)),
      data_consume(d1, pos));
  }
  (void) data_copy(data_range(d1, pos, pos + data_size(d2)), d2);
  return data_resize(d1, data_size(d1) + data_size(d2));
}

INLINE data_t data_erase(data_t d, size_t from, size_t to)
{
  (void) data_move(
    data_range(d, from, data_size(d) - (to - from)),
    data_consume(d, to));
  d = data_resize(d,  data_size(d) - (to - from));
  return d;
}

INLINE data_t data_append(const data_t d, const data_t append)
{
  return data_insert(d, data_size(d), append);
}

INLINE data_t data_alloc(size_t size)
{
  return data_realloc(data_null(), size);
}

INLINE data_t data_realloc(data_t d, size_t size)
{
  return data(realloc(data_base(d), size), data_size(d));
}

INLINE data_t data_free(data_t d)
{
  free(data_base(d));
  return data_null();
}

/* STRING */

enum
{
  __STRING_DECODE_ACCEPT = 0,
  __STRING_DECODE_REJECT = 12
};

INLINE string_t string(const char *chars)
{
  return data_string(chars);
}

INLINE uint8_t *string_base(const string_t s)
{
  return data_base(s);
}

INLINE size_t string_size(const string_t s)
{
  return data_size(s);
}

INLINE size_t string_length(const string_t s)
{
  string_t c;
  size_t length = 0;

  string_foreach(s, c)
    length++;
  return length;
}

INLINE string_t string_null(void)
{
  return data_null();
}

INLINE bool string_nullp(const string_t s)
{
  return string_base(s) == NULL;
}

INLINE bool string_emptyp(const string_t s)
{
  return string_size(s) == 0;
}

INLINE bool string_equalp(const string_t s1, const string_t s2)
{
  return data_equalp(s1, s2);
}

INLINE string_t string_find(const string_t s, const string_t match)
{
  return data_find(s, match);

}

INLINE uint32_t __string_decode_byte(uint32_t state, uint32_t *codepoint, uint8_t byte)
{
  static const uint8_t utf8d[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  7,  7,  7,  7,  7,  7,  7,  7,
    7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  8,  8,  2,  2,
    2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
    10, 3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  3,  3,  11, 6,  6,  6,  5,  8,  8,  8,  8,  8,  8,  8,
    8,  8,  8,  8,  0,  12, 24, 36, 60, 96, 84, 12, 12, 12, 48, 72, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
    12, 0,  12, 12, 12, 12, 12, 0,  12, 0,  12, 12, 12, 24, 12, 12, 12, 12, 12, 24, 12, 24, 12, 12, 12, 12, 12, 12,
    12, 12, 12, 24, 12, 12, 12, 12, 12, 24, 12, 12, 12, 12, 12, 12, 12, 24, 12, 12, 12, 12, 12, 12, 12, 12, 12, 36,
    12, 36, 12, 12, 12, 36, 12, 12, 12, 12, 12, 36, 12, 36, 12, 12, 12, 36, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12};
  uint32_t type = utf8d[byte];

  if (codepoint)
    *codepoint = (state != __STRING_DECODE_ACCEPT) ? (byte & 0x3fu) | (*codepoint << 6) : (0xff >> type) & (byte);
  state = utf8d[256 + state + type];
  return state;
}

INLINE string_t string_character(data_t storage, uint32_t codepoint)
{
  uint8_t *p = data_base(storage);

  if (codepoint < 0x80)
  {
    p[0] = codepoint;
    return data(p, 1);
  }
  else if (codepoint < 0x800)
  {
    p[0] = 0xc0 | (codepoint >> 6);
    p[1] = 0x80 | (codepoint & 0x3f);
    return data(p, 2);
  }
  else if (codepoint < 0x10000)
  {
    p[0] = 0xe0 | (codepoint >> 12);
    p[1] = 0x80 | ((codepoint >> 6) & 0x3f);
    p[2] = 0x80 | (codepoint & 0x3f);
    return data(p, 3);
  }
  else
  {
    p[0] = 0xf0 | (codepoint >> 18);
    p[1] = 0x80 | ((codepoint >> 12) & 0x3f);
    p[2] = 0x80 | ((codepoint >> 6) & 0x3f);
    p[3] = 0x80 | (codepoint & 0x3f);
    return data(p, 4);
  }
}

INLINE uint32_t string_codepoint(const string_t character)
{
  uint32_t codepoint = 0, state = __STRING_DECODE_ACCEPT;
  size_t i = 0;

  while (i < string_size(character))
  {
    state = __string_decode_byte(state, &codepoint, string_base(character)[i]);
    if (state == __STRING_DECODE_REJECT)
      break;
    i++;
    if (state == __STRING_DECODE_ACCEPT)
      return codepoint;
  }
  return 0xfffd;
}

INLINE string_t string_first(const string_t s)
{
  uint32_t state;
  size_t i;

  i = 0;
  state = __STRING_DECODE_ACCEPT;
  while (i < string_size(s))
  {
    state = __string_decode_byte(state, NULL, string_base(s)[i]);
    if (state == __STRING_DECODE_REJECT)
      break;
    i++;
    if (state == __STRING_DECODE_ACCEPT)
      return data_range(s, 0, i);
  }
  return string_null();
}

INLINE string_t string_rest(const string_t s, const string_t first)
{
  return data_consume(s, string_size(first));
}

INLINE bool string_valid(const string_t s)
{
  string_t first, rest = s;

  while (!string_emptyp(rest))
  {
    first = string_first(rest);
    if (string_nullp(first))
      return false;
    rest = string_rest(rest, first);
  }
  return true;
}

INLINE size_t string_fwrite(const string_t s, FILE *f)
{
  return fwrite(string_base(s), string_size(s), 1, f);
}

INLINE string_t string_copy(const string_t s)
{
  return string_append(string_null(), s);
}

NOINLINE string_t string_concat_vector(const vector_t strings)
{
  string_t s, e;
  size_t size;

  size = 0;
  vector_foreach(strings, string_t, e)
    size += string_size(e);

  s = data_alloc(size);
  vector_foreach(strings, string_t, e)
    s = data_append(s, e);

  return s;
}

NOINLINE string_t string_replace(const string_t s, const string_t match, const string_t replace)
{
  string_t rest, find, result;
  size_t count, offset;

  if (string_emptyp(match))
    return string_null();

  count = 0;
  rest = s;
  while (!string_emptyp(rest))
  {
    find = string_find(rest, match);
    if (!string_emptyp(find))
      count++;
    rest = data_consume(rest, data_offset(rest, find) + string_size(find));
  }

  result = data_alloc(string_size(s) + count * (string_size(replace) - string_size(match)));

  rest = s;
  while (!string_emptyp(rest))
  {
    find = string_find(rest, match);
    offset = data_offset(rest, find);
    result = data_append(result, data_range(rest, 0, offset));
    if (!string_emptyp(find))
      result = data_append(result, replace);
    rest = data_consume(rest, offset + string_size(find));
  }
  return result;
}

INLINE string_t string_insert(string_t s, size_t pos, const string_t insert)
{
  s = data_realloc(s, string_size(s) + string_size(insert));
  return data_insert(s, pos, insert);
}

INLINE string_t string_erase(string_t s, size_t from, size_t to)
{
  s = data_erase(s, from, to);
  return data_realloc(s, string_size(s));
}

INLINE string_t string_prepend(string_t s, const string_t prepend)
{
  return string_insert(s, 0, prepend);
}

INLINE string_t string_append(string_t s, const string_t append)
{
  return string_insert(s, string_size(s), append);
}

INLINE string_t string_free(string_t s)
{
  return data_free(s);
}

/* VECTOR */

INLINE void *vector_base(const vector_t v)
{
  return data_base(v);
}

INLINE size_t vector_size(const vector_t v)
{
  return data_size(v);
}

INLINE bool vector_emptyp(const vector_t v)
{
  return vector_size(v) == 0;
}

INLINE bool vector_equalp(const vector_t v1, const vector_t v2)
{
  return data_equalp(v1, v2);
}

INLINE vector_t vector_copy(const vector_t v)
{
  vector_t copy;

  copy = data_resize(data_alloc(data_roundup(vector_size(v))), vector_size(v));
  return data_copy(copy, v);
}

INLINE vector_t vector_insert_data(vector_t v, size_t pos, const data_t insert)
{
  size_t size_new;

  size_new = vector_size(v) + data_size(insert);
  if (size_new > data_roundup(vector_size(v)))
    v = data_realloc(v, data_roundup(size_new));
  return data_insert(v, pos, insert);
}

INLINE vector_t vector_erase_data(vector_t v, const data_t erase)
{
  size_t size_old = vector_size(v);

  v = data_erase(v, data_offset(v, erase), data_offset(v, erase) + data_size(erase));
  if (data_roundup(vector_size(v)) < size_old)
    v = data_realloc(v, data_roundup(vector_size(v)));
  return v;
}

INLINE vector_t vector_free(vector_t v)
{
  return data_free(v);
}

/* LIST */

INLINE list_t list_null(void)
{
  return (list_t) {.front = NULL, .length = 0};
}

INLINE size_t list_length(const list_t l)
{
  return l.length;
}

INLINE bool list_emptyp(const list_t l)
{
  return list_length(l) == 0;
}

INLINE list_node_t *list_node(const list_iter_t *iter)
{
  return (list_node_t *) iter - 1;
}

INLINE list_iter_t *list_next(const list_iter_t *iter)
{
  return list_node(iter)->next->element;
}

INLINE list_iter_t *list_prev(const list_iter_t *iter)
{
  return list_node(iter)->prev->element;
}

INLINE list_iter_t *list_front(const list_t l)
{
  return l.front;
}

INLINE list_iter_t *list_back(const list_t l)
{
  return l.front ? list_prev(l.front) : NULL;
}

INLINE list_t list_insert_node(list_t l, list_iter_t *iter, list_node_t *node)
{
  list_node_t *prev;

  if (list_emptyp(l))
  {
    node->prev = node;
    node->next = node;
  }
  else
  {
    prev = list_node(iter ? iter : list_back(l));
    node->prev = prev;
    node->next = prev->next;
    prev->next = node;
    node->next->prev = node;
  }
  if (!iter)
    l.front = node->element;
  l.length++;
  return l;
}

INLINE list_t list_insert_alloc(list_t l, list_iter_t *iter, size_t size, void **element)
{
  list_node_t *node;

  node = malloc(sizeof *node + size);
  *element = node->element;
  return list_insert_node(l, iter, node);
}

INLINE list_t list_insert_data(list_t l, list_iter_t *iter, data_t d)
{
  void *element;

  l = list_insert_alloc(l, iter, data_size(d), &element);
  data_copy(data(element, data_size(d)), d);
  return l;
}

INLINE list_t list_insert_vector(list_t l , list_iter_t *iter, size_t size, vector_t v)
{
  size_t length = vector_size(v) / size, i;

  for (i = 0; i < length; i++)
  {
    l = list_insert_data(l, iter, data(data_base(v) + (i * size), size));
    iter = iter ? list_next(iter) : list_front(l);
  }
  return l;
}

INLINE list_t list_remove_node(list_t l, list_node_t *node)
{
  node->prev->next = node->next;
  node->next->prev = node->prev;
  if (l.front == node->element)
  {
    l.front = node->next->element;
    if (l.front == node->element)
      l.front = NULL;
  }
  l.length--;
  return l;
}

INLINE list_t list_remove_data(list_t l, data_t d)
{
  list_iter_t *iter, *next;

  for (iter = list_front(l); iter; iter = next == list_front(l) ? NULL : next)
  {
    next = list_next(iter);
    if (data_equalp(data(iter, data_size(d)), d))
      l = list_erase(l, iter);
  }
  return l;
}

INLINE list_t list_remove_if(list_t l, bool (*cond)(list_iter_t *))
{
  list_iter_t *iter, *next;

  for (iter = list_front(l); iter; iter = next == list_front(l) ? NULL : next)
  {
    next = list_next(iter);
    if (cond(iter))
      l = list_erase(l, iter);
  }
  return l;
}

INLINE list_t list_erase(list_t l, list_iter_t *iter)
{
  list_node_t *node;

  node = list_node(iter);
  l = list_remove_node(l, node);
  free(node);
  return l;
}

INLINE list_t list_pop_back(list_t l)
{
  return list_erase(l, list_back(l));
}

INLINE list_t list_pop_front(list_t l)
{
  return list_erase(l, list_front(l));
}

INLINE list_t list_splice(list_t l, list_iter_t *iter, list_t *from, list_iter_t *from_iter)
{
  list_node_t *node = list_node(from_iter);

  *from = list_remove_node(*from, node);
  return list_insert_node(l, iter, node);
}

INLINE list_t list_free(list_t l)
{
  list_iter_t *iter, *next;

  for (iter = list_front(l); iter; iter = next == list_front(l) ? NULL : next)
  {
    next = list_next(iter);
    free(list_node(iter));
  }
  return list();
}

/* BUFFER */

INLINE buffer_t buffer(void)
{
  return (buffer_t) {.data = data_null(), .memory = data_null()};
}

INLINE data_t buffer_data(const buffer_t b)
{
  return b.data;
}

INLINE data_t buffer_memory(const buffer_t b)
{
  return b.memory;
}

INLINE size_t buffer_size(const buffer_t b)
{
  return data_size(b.data);
}

INLINE bool buffer_emptyp(const buffer_t b)
{
  return buffer_size(b) == 0;
}

INLINE size_t buffer_capacity(const buffer_t b)
{
  return data_size(b.memory);
}

INLINE buffer_t buffer_align(buffer_t b, size_t pos)
{
  b.data = data_move(data_resize(data_shift(b.memory, pos), data_size(b.data)), b.data);
  return b;
}

INLINE buffer_t buffer_reserve(buffer_t b, size_t capacity)
{
  size_t offset;

  if (capacity > buffer_capacity(b))
  {
    offset = data_offset(b.memory, b.data);
    capacity = data_roundup(capacity);
    b.memory = data_resize(data_realloc(b.memory, capacity), capacity);
    b.data = data_resize(data_shift(b.memory, offset), data_size(b.data));
  }
  return b;
}

INLINE buffer_t buffer_alloc(buffer_t b, size_t pos, size_t size, void **base)
{
  b = buffer_reserve(b, buffer_size(b) + size);
  if (pos)
  {
    if (size > buffer_capacity(b) - buffer_size(b) - data_offset(b.memory, b.data))
      b = buffer_align(b, 0);
    if (pos < buffer_size(b))
      data_move(data_shift(data_consume(b.data, pos), size), data_consume(b.data, pos));
  }
  else if (buffer_size(b))
  {
    if (size > data_offset(b.memory, b.data))
      b = buffer_align(b, buffer_capacity(b) - buffer_size(b));
    b.data = data_shift(b.data, -size);
  }
  b.data = data_resize(b.data, data_size(b.data) + size);
  *base = data_base(b.data) + pos;
  return b;
}

INLINE buffer_t buffer_insert(buffer_t b, size_t pos, const data_t insert)
{
  void *base;

  b = buffer_alloc(b, pos, data_size(insert), &base);
  data_copy(data(base, data_size(insert)), insert);
  return b;
}

INLINE buffer_t buffer_prepend(buffer_t b, const data_t prepend)
{
  return buffer_insert(b, 0, prepend);
}

INLINE buffer_t buffer_append(buffer_t b, const data_t append)
{
  return buffer_insert(b, buffer_size(b), append);
}

INLINE buffer_t buffer_erase(buffer_t b, size_t from, size_t to)
{
  if (from)
    b.data = data_erase(b.data, from, to);
  else
    b.data = data_consume(b.data, to);
  if (data_emptyp(b.data))
    b = buffer_clear(b);
  return b;
}

INLINE buffer_t buffer_clear(buffer_t b)
{
  b.data = data_resize(b.memory, 0);
  return b;
}

INLINE buffer_t buffer_free(buffer_t b)
{
  (void) data_free(b.memory);
  return buffer();
}

/* POOL */

INLINE pool_t pool(void)
{
  return buffer();
}

INLINE pool_t pool_free(pool_t p)
{
  void *memory;

  vector_foreach(buffer_data(p), void *, memory)
    free(memory);
  return buffer_free(p);
}

INLINE pool_t pool_alloc(pool_t p, void **memory, size_t size)
{
  if (dynamic_unlikely(buffer_emptyp(p)))
    p = pool_return(p, malloc(size));
  *memory = vector_back(buffer_data(p), void *);
  return buffer_erase(p, buffer_size(p) - sizeof (void *), buffer_size(p));
}

INLINE pool_t pool_return(pool_t p, void *memory)
{
  return buffer_append(p, vector(void *, memory));
}

#endif /* DYNAMIC_H */
