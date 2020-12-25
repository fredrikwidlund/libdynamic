#include <stdio.h>
#include <assert.h>

#include <dynamic.h>

static int compare(void *p1, void *p2)
{
  return *(int *) p1 - *(int *) p2;
}

int main()
{
  list list;
  int *p;

  list_construct(&list);

  list_push_back(&list, (int[]){4}, sizeof (int));
  list_push_back(&list, (int[]){5}, sizeof (int));
  list_push_back(&list, (int[]){6}, sizeof (int));
  list_push_front(&list, (int[]){3}, sizeof (int));
  list_push_front(&list, (int[]){2}, sizeof (int));
  list_push_front(&list, (int[]){1}, sizeof (int));

  printf("foreach\n");
  list_foreach(&list, p)
    printf("%d\n", *p);

  printf("foreach reverse\n");
  list_foreach_reverse(&list, p)
    printf("%d\n", *p);

  p = list_find(&list, compare, (int[]){1});
  printf("found %d\n", *p);
  list_insert(p, (int[]){-2}, sizeof (int));
  list_insert(p, (int[]){0}, sizeof (int));
  list_insert(list_front(&list), (int[]){-1}, sizeof (int));
  list_insert(list_end(&list), (int[]){7}, sizeof (int));

  printf("[forward]\n");
  list_foreach(&list, p)
    printf("%d\n", *p);

  printf("[backward]\n");
  list_foreach_reverse(&list, p)
    printf("%d\n", *p);

  list_destruct(&list, NULL);
}
