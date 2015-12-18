#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "buffer.h"
#include "vector.h"
#include "string.h"

/* allocators */

string *string_new(char *data)
{
  string *s;

  s = malloc(sizeof *s);
  if (!s)
    return NULL;

  string_init(s, data);

  return s;
}

void string_free(string *s)
{
  buffer_clear(&s->buffer);
  free(s);
}

void string_init(string *s, char *data)
{
  buffer_init(&s->buffer);
  buffer_insert(&s->buffer, 0, data, strlen(data) + 1);
}

char *string_deconstruct(string *s)
{
  return buffer_deconstruct(&s->buffer);
}

/* capacity */

size_t string_length(string *s)
{
  return buffer_size(&s->buffer) - 1;
}

size_t string_capacity(string *s)
{
  return buffer_capacity(&s->buffer) - 1;
}

int string_reserve(string *s, size_t size)
{
  return buffer_reserve(&s->buffer, size + 1);
}

void string_clear(string *s)
{
  buffer_clear(&s->buffer);
  buffer_insert(&s->buffer, 0, "", 1);
}

int string_empty(string *s)
{
  return string_length(s) == 0;
}

int string_shrink_to_fit(string *s)
{
  return buffer_compact(&s->buffer);
}

/* modifiers */

int string_prepend(string *s, char *data)
{
  return buffer_insert(&s->buffer, 0, data, strlen(data));
}

int string_append(string *s, char *data)
{
  return buffer_insert(&s->buffer, string_length(s), data, strlen(data));  
}

int string_insert(string *s, size_t pos, char *data)
{
  if (pos > string_length(s))
    return -1;

  return buffer_insert(&s->buffer, pos, data, strlen(data));
}

int string_erase(string *s, size_t pos, size_t size)
{
  size_t l = string_length(s);

  if (pos > l)
    return -1;

  buffer_erase(&s->buffer, pos, size > l - pos ? l - pos : size);

  return 0;
}

int string_replace(string *s, size_t pos, size_t len, char *data)
{
  int e;

  e = string_erase(s, pos, len);
  if (e == -1)
    return -1;

  return string_insert(s, pos, data);
}

int string_replace_all(string *s, char *find, char *sub)
{
  size_t i;
  int e;

  for (i = string_find(s, find, 0); i != (size_t) -1; i = string_find(s, find, i + strlen(sub)))
    {
      e = string_replace(s, i, strlen(find), sub);
      if (e == -1)
        return -1;
    }

  return 0;
}


/* string operations */

char *string_data(string *s)
{
  return buffer_data(&s->buffer);
}

int string_copy(string *s, char *data, size_t len, size_t pos, size_t *size)
{
  size_t l = string_length(s);

  if (pos > l)
    return -1;

  *size = l - pos < len ? l - pos : len;
  memcpy(data, string_data(s), *size);

  return 0;
}

size_t string_find(string *s, char *data, size_t pos)
{
  char *p;

  if (pos > string_length(s))
    return -1;

  p = strstr(string_data(s) + pos, data);
  return p ? p - string_data(s) : -1;
}

string *string_substr(string *s, size_t pos, size_t len)
{
  string *result;
  size_t n, l = string_length(s);

  if (pos > l)
    return NULL;

  result = malloc(sizeof *result);
  if (!result)
    return NULL;

  n = l - pos < len ? l - pos : len;

  buffer_init(&result->buffer);
  buffer_reserve(&result->buffer, n + 1);
  buffer_insert(&result->buffer, 0, string_data(s) + pos, n);
  string_data(result)[n] = 0;

  return result;
}

int string_compare(string *s1, string *s2)
{
  return strcmp(string_data(s1), string_data(s2));
}

vector *string_split(string *s, char *delim)
{
  vector *v;
  char *token;
  string *copy = string_new(string_data(s));
  string *part;

  v = vector_new(sizeof(string *));
  vector_release(v, string_split_release);
  for (token = strtok(string_data(copy), delim); token; token = strtok(NULL, delim))
    {
      part = string_new(token);
      vector_push_back(v, &part);
    }
  string_free(copy);

  return v;
}

void string_split_release(void *object)
{
  string_free(*(string **) object);
}
