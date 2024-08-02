#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>

#include "dynamic/list.h"

void test_list(__attribute__((unused)) void **arg)
{
  list_t *l = list_create(), l2;
  int *i;

  list_prepend(l, (int []){1}, sizeof (int));
  list_append(l, (int []){2}, sizeof (int));
  list_append(l, (int []){3}, sizeof (int));
  list_erase(list_next(list_front(l)));
  list_insert(list_back(l), (int []){0}, sizeof (int));
  assert_false(list_empty(l));

  l2 = list(&l2);
  i = list_alloc(list_front(&l2), sizeof i);
  *i = 42;
  list_splice(list_front(l), i);
  list_splice(i, i);

  list_foreach(l, i)
    printf("%d\n", *i);
  list_foreach_reverse(l, i)
    printf("%d\n", *i);

  list_clear(l);
  assert_true(list_prev(list_back(l)) == list_next(list_front(l)));
  list_destroy(l);
}

int main()
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_list)
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
