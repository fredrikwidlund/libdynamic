#ifndef LIST_H_INCLUDED
#define LIST_H_INCLUDED

#define list_foreach(i, l, o) \
  for ((i) = 0, (o) = list_front(l); (size_t) (i) < list_size(l); (i) ++, (o) = list_next(o))
#define list_foreach_reverse(i, l, o) \
  for ((i) = list_size(l) - 1, (o) = list_back(l); (i) >= 0; (i) --, (o) = list_previous(o))

typedef struct list_item list_item;
typedef struct list list;

struct list_item
{
  list_item  *previous;
  list_item  *next;
  char        object[];
};

struct list
{
  list_item  *front;
  list_item  *back;
  size_t      size;
  size_t      object_size;
  void      (*object_release)(void *);
};

/* constructor/destructor */
void    list_construct(list *, size_t);
void    list_object_release(list *, void (*)(void *));
void    list_destruct(list *);

/* iterators */
void   *list_next(void *);
void   *list_previous(void *);

/* capacity */
size_t  list_size(list *);
int     list_empty(list *);

/* element access */
void   *list_front(list *);
void   *list_back(list *);

/* modifiers */
void    list_push_front(list *, void *);
void    list_push_back(list *, void *);
void    list_insert_before(list *, void *, void *);
void    list_insert_after(list *, void *, void *);
void    list_erase(list *, void *);
void    list_clear(list *);

/* operations */
void   *list_find(list *, int (*)(void *, void *), void *);

#endif /* LIST_H_INCLUDED */
