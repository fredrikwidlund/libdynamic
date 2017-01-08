#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "buffer.h"
#include "vector.h"
#include "string.h"

static void string_split_release(void *object)
{
  string_destruct((string *) object);
}

/* constructor/destructor */

void string_construct(string *s)
{
  buffer_construct(&s->buffer);
  buffer_insert(&s->buffer, 0, "", 1);
}

void string_destruct(string *s)
{
  buffer_destruct(&s->buffer);
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

void string_reserve(string *s, size_t size)
{
  buffer_reserve(&s->buffer, size + 1);
}

int string_empty(string *s)
{
  return string_length(s) == 0;
}

void string_shrink_to_fit(string *s)
{
  buffer_compact(&s->buffer);
}

/* modifiers */

void string_insert(string *s, size_t pos, char *data)
{
  string_insert_buffer(s, pos, data, strlen(data));
}

void string_insert_buffer(string *s, size_t pos, char *data, size_t size)
{
  buffer_insert(&s->buffer, pos, data, size);
}

void string_prepend(string *s, char *data)
{
  buffer_insert(&s->buffer, 0, data, strlen(data));
}

void string_append(string *s, char *data)
{
  buffer_insert(&s->buffer, string_length(s), data, strlen(data));
}

void string_erase(string *s, size_t pos, size_t size)
{
  buffer_erase(&s->buffer, pos, size);
}

void string_replace(string *s, size_t pos, size_t size, char *data)
{
  string_erase(s, pos, size);
  string_insert(s, pos, data);
}

void string_replace_all(string *s, char *find, char *sub)
{
  ssize_t i;

  for (i = string_find(s, find, 0); i >= 0; i = string_find(s, find, i + strlen(sub)))
    string_replace(s, i, strlen(find), sub);
}

void string_clear(string *s)
{
  buffer_clear(&s->buffer);
  string_construct(s);
}

/* string operations */

char *string_data(string *s)
{
  return buffer_data(&s->buffer);
}

ssize_t string_find(string *s, char *data, size_t pos)
{
  char *p;

  p = strstr(string_data(s) + pos, data);
  return p ? p - string_data(s) : -1;
}

int string_compare(string *s1, string *s2)
{
  return strcmp(string_data(s1), string_data(s2));
}

void string_split(string *s, char *delim, vector *v)
{
  string copy, token;
  char *cp, *cp_saved;

  vector_construct(v, sizeof(string));
  vector_object_release(v, string_split_release);
  string_construct(&copy);
  string_append(&copy, string_data(s));
  for (cp = strtok_r(string_data(&copy), delim, &cp_saved); cp; cp = strtok_r(NULL, delim, &cp_saved))
    {
      string_construct(&token);
      string_append(&token, cp);
      vector_push_back(v, &token);
    }
  string_destruct(&copy);
}
