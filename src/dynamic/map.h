#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

#ifndef MAP_ELEMENTS_CAPACITY_MIN
#define MAP_ELEMENTS_CAPACITY_MIN 16
#endif /* MAP_ELEMENTS_CAPACITY_MIN */

typedef struct map map;
struct map
{
  void   *elements;
  size_t  elements_count;
  size_t  elements_capacity;
  size_t  element_size;
  void   *element_empty;
};

/* constructor/destructor */

void    map_construct(map *, size_t, void *);
void    map_destruct(map *, int (*)(void *, void *), void (*)(void *));

/* capacity */

size_t  map_size(map *);
void    map_reserve(map *, size_t, size_t (*)(void *), int (*)(void *, void *));

/* element access */

void   *map_element_empty(map *);
void   *map_at(map *, void *, size_t (*)(void *), int (*)(void *, void *));

/* modifiers */
void    map_insert(map *, void *, size_t (*)(void *), int (*)(void *, void *), void (*)(void *));
void    map_clear(map *, int (*)(void *, void *), void (*)(void *));
void    map_erase(map *, void *, size_t (*)(void *), int (*)(void *, void *), void (*)(void *));

#endif /* MAP_H_INCLUDED */
