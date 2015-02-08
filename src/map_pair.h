#ifndef MAP_PAIR_H_INCLUDED
#define MAP_PAIR_H_INCLUDED

#define MAP_PAIR_DELETED         ((uint64_t) -1)
#define MAP_PAIR_EMPTY           ((uint64_t) -2)
#define MAP_PAIR_MAX_LOAD_FACTOR 0.5

#ifdef HAVE_BUILTIN_EXPECT
#define __unlikely(cond) __builtin_expect(!!(cond), 0)
#define __likely(cond)   __builtin_expect(!!(cond), 1)
#else
#define __unlikely(cond) (cond)
#define __likely(cond)   (cond)
#endif /* HAVE_BUILTIN_EXPECT */

//#define map_pair_insert_rvalue(m, k, v) do {__typeof__(v) __value[] = {(v)}; map_pair_insert((m), (k), __value);} while (0)

typedef struct map_pair map_pair;

struct map_pair
{
  uint64_t  *keys;
  uint64_t  *values;
  size_t     size;
  size_t     deleted;
  size_t     capacity;
  size_t     watermark;
  double     max_load_factor;
  void     (*release)(uint64_t, uint64_t);  
};

/* allocators */
map_pair *map_pair_new();
void      map_pair_init(map_pair *);
void      map_pair_release(map_pair *, void (*)(uint64_t, uint64_t));
void      map_pair_free(map_pair *);

/* capacity */
size_t    map_pair_size(map_pair *);

/* element access */
uint64_t  map_pair_get(map_pair *, size_t);
void      map_pair_put(map_pair *, size_t, uint64_t, uint64_t);
uint64_t  map_pair_at(map_pair *, uint64_t);

/* element lookup */
size_t    map_pair_find(map_pair *, uint64_t);

/* modifiers */
void      map_pair_insert(map_pair *, uint64_t, uint64_t);
void      map_pair_erase(map_pair *, uint64_t);
void      map_pair_clear(map_pair *);

/* buckets */
size_t    map_pair_bucket_count(map_pair *);

/* hash policy */
void      map_pair_max_load_factor(map_pair *, double);
void      map_pair_rehash(map_pair *, size_t);
void      map_pair_reserve(map_pair *, size_t);

/* iterators */
size_t    map_pair_begin(map_pair *);
size_t    map_pair_next(map_pair *, size_t);
size_t    map_pair_end(map_pair *);

/* internals */
size_t    map_pair_find_free(map_pair *, uint64_t);
size_t    map_pair_next_inclusive(map_pair *, size_t);
size_t    map_pair_roundup_size(size_t);

#endif /* MAP_PAIR_H_INCLUDED */
