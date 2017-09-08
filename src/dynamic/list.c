#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "list.h"

/* internals */

static list_item *object_list_item(void *object)
{
  return (list_item *) ((uintptr_t) object - offsetof(list_item, object));
}

static list_item *list_item_new(list *l, void *object)
{
  list_item *item;

  item = malloc(sizeof (list_item) + l->object_size);
  memcpy(item->object, object, l->object_size);

  return item;
}

/* constructor/destructor */

void list_construct(list *l, size_t object_size)
{
  *l = (list) {.object_size = object_size};
}

void list_object_release(list *l, void (*release)(void *))
{
  l->object_release = release;
}

void list_destruct(list *l)
{
  list_clear(l);
}

/* iterators */

void *list_next(void *object)
{
  return object_list_item(object)->next->object;
}

void *list_previous(void *object)
{
  return object_list_item(object)->previous->object;
}

/* capacity */

size_t list_size(list *l)
{
  return l->size;
}

int list_empty(list *l)
{
  return l->size == 0;
}

/* element access */

void *list_front(list *l)
{
  return l->front->object;
}

void *list_back(list *l)
{
  return l->back->object;
}

/* modifiers */

void list_push_front(list *l, void *object)
{
  list_item *item = list_item_new(l, object);

  if (list_empty(l))
    {
      item->next = item;
      item->previous = item;
      l->front = item;
      l->back = item;
    }
  else
    {
      item->next = l->front;
      item->previous = l->back;
      l->front->previous = item;
      l->back->next = item;
      l->front = item;
    }

  l->size++;
}

void list_push_back(list *l, void *object)
{
  list_item *item = list_item_new(l, object);

  if (list_empty(l))
    {
      item->next = item;
      item->previous = item;
      l->front = item;
      l->back = item;
    }
  else
    {
      item->next = l->front;
      item->previous = l->back;
      l->front->previous = item;
      l->back->next = item;
      l->back = item;
    }

  l->size++;
}

void list_insert_before(list *l, void *list_object, void *object)
{
  list_item *next = object_list_item(list_object), *item = list_item_new(l, object);

  item->next = next;
  item->previous = next->previous;
  next->previous->next = item;
  next->previous = item;

  if (l->front == next)
    l->front = item;

  l->size ++;
}

void list_insert_after(list *l, void *list_object, void *object)
{
  list_item *previous = object_list_item(list_object), *item = list_item_new(l, object);

  item->next = previous->next;
  item->previous = previous;
  previous->next->previous = item;
  previous->next = item;

  if (l->back == previous)
    l->back = item;

  l->size ++;
}

void list_erase(list *l, void *object)
{
  list_item *item = object_list_item(object);

  item->previous->next = item->next;
  item->next->previous = item->previous;

  if (l->front == item)
    l->front = item->next;
  if (l->back == item)
    l->back = item->previous;

  l->size --;

  if (l->object_release)
    l->object_release(object);

  free(item);
}

void list_clear(list *l)
{
  while (!list_empty(l))
    list_erase(l, list_front(l));
}

/* operations */

void *list_find(list *l, int (*equal)(void *, void *), void *object)
{
  list_item *item;

  if (list_empty(l))
    return NULL;

  item = l->front;
  while (1)
    {
      if (equal(item->object, object))
        return item->object;
      item = item->next;
      if (item == l->front)
        break;
    }

  return NULL;
}
