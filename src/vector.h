#ifndef VECTOR_H_INCLUDED
#define VECTOR_H_INCLUDED

typedef struct vector vector;

struct vector
{
  void    *data;
  size_t   size;
  size_t   capacity;
  size_t   object_size;
  void   (*release)(void *);
};

/* allocators */
vector *vector_new(size_t);
void    vector_free(vector *);
void    vector_init(vector *, size_t);
void    vector_release(vector *, void (*)(void *));
void   *vector_deconstruct(vector *);

/* capacity */
size_t  vector_size(vector *);
int     vector_resize(vector *, size_t);
size_t  vector_capacity(vector *);
int     vector_empty(vector *);
int     vector_reserve(vector *, size_t);
int     vector_shrink_to_fit(vector *);

/* element access */
void   *vector_at(vector *, size_t);
void   *vector_front(vector *);
void   *vector_back(vector *);
void   *vector_data(vector *);

/* modifiers */
int     vector_push_back(vector *, void *);
void    vector_pop_back(vector *);
int     vector_insert(vector *, size_t, size_t, void *);
void    vector_erase(vector *, size_t, size_t);
int     vector_clear(vector *);

/* internals */
size_t  vector_roundup_size(size_t);

#endif /* VECTOR_H_INCLUDED */
