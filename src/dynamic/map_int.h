#ifndef MAP_INT_H_INCLUDED
#define MAP_INT_H_INCLUDED

typedef struct map_int map_int;
struct map_int
{
  map                   map;
  void                 *empty;
  map_release_callback  release;
};

/* constructor/destructor */

void    map_int_construct(map_int *, size_t);
void    map_int_destruct(map_int *);

/* capacity */

size_t  map_int_size(map_int *);
void    map_int_reserve(map_int *, size_t);

/* element access */

int     map_int_element_empty(map_int *);
void   *map_int_at(map_int *, int);

/* modifiers */

void    map_int_insert(map_int *, void *);
void    map_int_erase(map_int *, int);
void    map_int_clear(map_int *);

#endif /* MAP_INT_H_INCLUDED */
