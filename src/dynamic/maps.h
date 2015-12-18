#ifndef MAPS_H_INCLUDED
#define MAPS_H_INCLUDED

#define MAPS_MAX_LOAD_FACTOR        0.5
#define MAPS_RESERVED_EMPTY         NULL
#define MAPS_RESERVED_DELETED       ((char *) 1)

typedef struct maps_key maps_key_t;
typedef struct maps maps;

struct maps
{
  char   **keys;
  void    *values;
  size_t   value_size;
  size_t   size;
  size_t   deleted;
  size_t   capacity;
  size_t   watermark;
  double   max_load_factor;
  void   (*release)(char *, void *);
};

/* allocators */
maps   *maps_new(size_t);
void    maps_init(maps *, size_t);
void    maps_release(maps *, void (*)(char *, void *));
void    maps_free(maps *);

/* capacity */
size_t  maps_size(maps *);

/* element access */
void   *maps_get(maps *, size_t);
void    maps_put(maps *, size_t, char *, void *);
void   *maps_at(maps *, char *);

/* element lookup */
size_t  maps_find(maps *, char *);

/* modifiers */
void    maps_insert(maps *, char *, void *);
void    maps_erase(maps *, char *);
void    maps_clear(maps *);

/* buckets */
size_t  maps_bucket_count(maps *);

/* hash policy */
void    maps_max_load_factor(maps *, double);
void    maps_rehash(maps *, size_t);
void    maps_reserve(maps *, size_t);

/* iterators */
size_t  maps_begin(maps *);
size_t  maps_next(maps *, size_t);
size_t  maps_end(maps *);

/* internals */
size_t  maps_find_free(maps *, char *);
size_t  maps_next_inclusive(maps *, size_t);
size_t  maps_roundup_size(size_t);
void   *maps_data_offset(void *, size_t);

#endif /* MAPS_H_INCLUDED */
