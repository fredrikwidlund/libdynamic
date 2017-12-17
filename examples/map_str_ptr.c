#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/param.h>
#include <time.h>
#include <err.h>

#include "dynamic.h"
#include "map_str_ptr.h"

struct map_str_ptr_element
{
  char *key;
  void *value;
};

static map_str_ptr_element empty = {.key = NULL};

static size_t hash(map *m, void *e)
{
  (void) m;
  return hash_string(((map_str_ptr_element *)e)->key);
}

static void set(map *m, void *dst, void *src)
{
  map_str_ptr_element *d = dst, *s = src;

  (void) m;
  *d = *s;
}

static int equal(map *m, void *p1, void *p2)
{
  map_str_ptr_element *e1 = p1, *e2 = p2;

  (void) m;
  return e1->key == e2->key || (e1->key && e2->key && strcmp(e1->key, e2->key) == 0);
}

void map_str_ptr_construct(map_str_ptr *m)
{
  map_construct(m, sizeof(map_str_ptr_element), &empty, set);
}

void map_str_ptr_destruct(map_str_ptr *m)
{
  map_destruct(m, NULL, NULL);
}

void *map_str_ptr_at(map_str_ptr *m, char *key)
{
  return ((map_str_ptr_element *) map_at(m, (map_str_ptr_element[]){{.key = key}}, hash, equal))->value;
}

void map_str_ptr_insert(map_str_ptr *m, char *key, void *value)
{
  map_insert(m,(map_str_ptr_element[]){{.key = key, .value = value}}, hash, equal, set, NULL);
}

void map_str_ptr_erase(map_str_ptr *m, char *key)
{
  map_erase(m,(map_str_ptr_element[]){{.key = key}}, hash, equal, set, NULL);
}

size_t map_str_ptr_size(map_str_ptr *m)
{
  return map_size(m);
}
