#ifndef BUFFER_H_INCLUDED
#define BUFFER_H_INCLUDED

typedef struct buffer buffer;

struct buffer
{
  char *base;
  char *top;
  char *memory_base;
  char *memory_top;
};

/* allocators */

buffer *buffer_new();
void    buffer_free(buffer *);
void    buffer_init(buffer *);
char   *buffer_deconstruct(buffer *);

/* capacity */

size_t  buffer_size(buffer *);
size_t  buffer_capacity(buffer *);
size_t  buffer_capacity_head(buffer *);
size_t  buffer_capacity_tail(buffer *);
int     buffer_reserve(buffer *, size_t);
int     buffer_compact(buffer *);

/* modifiers */

int     buffer_prepend(buffer *, char *, size_t);
int     buffer_append(buffer *, char *, size_t);
void    buffer_insert(buffer *, size_t, char *, size_t);
void    buffer_clear(buffer *);

/* element access */

char   *buffer_data(buffer *);
char   *buffer_end(buffer *);

/* internals */

size_t buffer_roundup(size_t);

#endif /* BUFFER_H_INCLUDED */
