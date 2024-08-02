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
    if (n <= 0)
      break;
    if (line[n - 1] == '\n')
    {
      line[n - 1] = 0;
      n--;
    }
    buffer_prepend(&b, data_string("\n"));
    buffer_prepend(&b, data(line, n));
  }

  fwrite(buffer_base(b), buffer_size(b), 1, stdout);
  buffer_clear(&b);
  free(line);
}
