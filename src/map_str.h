#ifndef MAP_STR_H_INCLUDED
#define MAP_STR_H_INCLUDED

#define MAP_STR_MAX_LOAD_FACTOR        0.5
#define MAP_STR_RESERVED_EMPTY         NULL
#define MAP_STR_RESERVED_DELETED       ((char *) 1)

//#define map_str_insert_rvalue(m, k, v) do {__typeof__(v) __value[] = {(v)}; map_str_insert((m), (k), __value);} while (0)

typedef struct map_str_key map_str_key_t;
typedef struct map_str map_str;

struct map_str
{
  char           **keys;
  void            *values;
  size_t           value_size;
  size_t           size;
  size_t           deleted;
  size_t           capacity;
  size_t           watermark;
  double           max_load_factor;
  void           (*release)(char *, void *);  
};

/* allocators */
map_str        *map_str_new(size_t);
void               map_str_init(map_str *, size_t);
void               map_str_release(map_str *, void (*)(char *, void *));
void               map_str_free(map_str *);

/* capacity */
size_t             map_str_size(map_str *);

/* element access */
void              *map_str_get(map_str *, size_t);
void               map_str_put(map_str *, size_t, char *, void *);
void              *map_str_at(map_str *, char *);

/* element lookup */
size_t             map_str_find(map_str *, char *);

/* modifiers */
void               map_str_insert(map_str *, char *, void *);
void               map_str_erase(map_str *, char *);
void               map_str_clear(map_str *);

/* buckets */
size_t             map_str_bucket_count(map_str *);

/* hash policy */
void               map_str_max_load_factor(map_str *, double);
void               map_str_rehash(map_str *, size_t);
void               map_str_reserve(map_str *, size_t);

/* iterators */
size_t             map_str_begin(map_str *);
size_t             map_str_next(map_str *, size_t);
size_t             map_str_end(map_str *);

/* internals */
size_t             map_str_find_free(map_str *, char *);
size_t             map_str_next_inclusive(map_str *, size_t);
size_t             map_str_roundup_size(size_t);
void              *map_str_data_offset(void *, size_t);

#endif /* MAP_STR_H_INCLUDED */
