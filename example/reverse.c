#include <stdio.h>
#include <dynamic.h>

int main()
{
  buffer_t b = buffer();
  char *line = NULL;
  size_t size = 0;
  ssize_t n;

  while (1)
  {
    n = getline(&line, &size, stdin);
    if (n == -1)
      break;
    b = buffer_prepend(b, data(line, n));
  }
  free(line);
  string_fwrite(buffer_data(b), stdout);
  buffer_free(b);
}
