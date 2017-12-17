#ifndef MAP_STR_PTR_H_INCLUDED
#define MAP_STR_PTR_H_INCLUDED

typedef map map_str_ptr;
typedef struct map_str_ptr_element map_str_ptr_element;

void    map_str_ptr_construct(map_str_ptr *);
void    map_str_ptr_destruct(map_str_ptr *);
void   *map_str_ptr_at(map_str_ptr *, char *);
void    map_str_ptr_insert(map_str_ptr *, char *, void *);
void    map_str_ptr_erase(map_str_ptr *, char *);
size_t  map_str_ptr_size(map_str_ptr *);

#endif /* MAP_STR_PTR_H_INCLUDED */
