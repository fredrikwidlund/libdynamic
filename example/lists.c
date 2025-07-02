#include <stdio.h>
#include <dynamic.h>

int main(int argc, char **argv)
{
  list_t l = list();
  int i = 0;
  char *s;

  l = list_insert_vector(l, NULL, sizeof(char *), data(argv, argc * sizeof *argv));
  printf("arguments: %lu\n", list_length(l));
  list_foreach(l, char *, s)
    printf("argv[%d] = [%s]\n", i++, s);
  l = list_free(l);
}
