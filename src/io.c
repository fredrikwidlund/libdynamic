#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "dynamic/buffer.h"
#include "dynamic/vector.h"
#include "dynamic/string.h"
#include "dynamic/io.h"

buffer *io_read(int fd, size_t size)
{
  buffer *m;
  struct stat st;
  char block[65536], *buffer;
  size_t block_size;
  ssize_t n;
  int e, in_memory;

  m = buffer_new();

  if (size == 0)
    {
      e = fstat(fd, &st);
      if (e == -1)
        {
          buffer_free(m);
          return NULL;
        }
      
      size = st.st_size;
      buffer_reserve(m, size);
      buffer = buffer_data(m);
      block_size = size;
      in_memory = 1;
    }
  else
    {
      buffer = block;
      block_size = sizeof block;
      in_memory = 0;
    }
  
  while (size)
    {
      n = read(fd, buffer, size > block_size ? block_size : size);
      if (n == -1)
        {
          buffer_free(m);
          return NULL;
        }
      
      if (n == 0)
        break;

      if (in_memory)
        {
          buffer += n;
          block_size -= n;
          m->size += n;
        }
      else
        {
          buffer_insert(m, buffer_size(m), block, n);
        }

      size -= n;
    }

  buffer_compact(m);

  return m;
}

int io_write(int fd, buffer *m)
{
  char *buffer;
  ssize_t n;
  size_t size = buffer_size(m);

  buffer = buffer_data(m);
  while (size)
    {
      n = write(fd, buffer, size);
      if (n == -1)
        return -1;

      buffer += n;
      size -= n;
    }

  return 0;
}

int io_writefile(buffer *m, char *filename)
{
  int fd, e;

  fd = open(filename, O_WRONLY);
  if (fd == -1)
    return -1;

  e = io_write(fd, m);
  (void) close(fd);
  
  return e;
}

buffer *io_buffer_load(char *filename)
{
  buffer *m;
  int fd;

  fd = open(filename, O_RDONLY);
  if (fd == -1)
    return NULL;
  
  m = io_read(fd, 0);
  (void) close(fd);

  return m;
}

string *io_string_load(char *path)
{
  buffer *b;

  b = io_buffer_load(path);
  if (!b)
    return NULL;

  buffer_insert(b, buffer_size(b), "", 1);

  return (string *) b;
}

string *io_string_get(int fd)
{
  string *s;
  ssize_t n;
  char one[2] = {0, 0};

  s = string_new("");

  while (1)
    {
      n = read(fd, one, 1);
      if (n == -1)
        {
          string_free(s);
          return NULL;
        }

      if (n == 0 || one[0] == '\n')
        return s;

      string_append(s, one);
    }
}
