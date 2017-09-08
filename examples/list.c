#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include "dynamic.h"

static int equal(void *p1, void *p2)
{
  return *(int *) p1 == *(int *) p2;
}

int main()
{
  list list;
  int *p;
  ssize_t i;

  list_construct(&list, sizeof (int));

  list_push_back(&list, (int[]){4});
  list_push_back(&list, (int[]){5});
  list_push_back(&list, (int[]){6});
  list_push_front(&list, (int[]){3});
  list_push_front(&list, (int[]){2});
  list_push_front(&list, (int[]){1});

  printf("foreach\n");
  list_foreach(i, &list, p)
    printf("%lu %d\n", i, *p);

  printf("foreach reverse\n");
  list_foreach_reverse(i, &list, p)
    printf("%lu %d\n", i, *p);

  p = list_find(&list, equal, (int[]){1});
  printf("found %d\n", *p);
  list_insert_before(&list, p, (int[]){-2});
  list_insert_before(&list, p, (int[]){0});

  list_insert_before(&list, list_front(&list), (int[]){-1});
  list_insert_after(&list, list_back(&list), (int[]){7});

  printf("[forward]\n");
  list_foreach(i, &list, p)
    printf("%lu %d\n", i, *p);

  printf("[backward]\n");
  list_foreach_reverse(i, &list, p)
    printf("%lu %d\n", i, *p);

  list_destruct(&list);
}
