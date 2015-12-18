#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/param.h>

#include "dynamic/cfarmhash.h"
#include "dynamic/map_str.h"

/* allocators */

map_str *map_str_new(size_t value_size)
{
  map_str *map;
  
  map = malloc(sizeof *map);
  map_str_init(map, value_size);
  
  return map;
}

void map_str_init(map_str *map, size_t value_size)
{
  memset(map, 0, sizeof *map);
  map->value_size = value_size;
  map_str_max_load_factor(map, MAP_STR_MAX_LOAD_FACTOR);
}

void map_str_release(map_str *map, void (*release)(char *, void *))
{
  map->release = release;
}

void map_str_free(map_str *map)
{
  map_str_clear(map);
  free(map);
}

/* capacity */

size_t map_str_size(map_str *map)
{
  return map->size;
}

/* Element access */

void *map_str_get(map_str *map, size_t position)
{
  return map_str_data_offset(map->values, position * map->value_size);
}

void map_str_put(map_str *map, size_t position, char *key, void *value)
{
  if (map->keys[position] == MAP_STR_RESERVED_DELETED)
    map->deleted --;
  map->keys[position] = key;
  memcpy(map_str_get(map, position), value, map->value_size);
  map->size ++;
}

void *map_str_at(map_str *map, char *key)
{
  size_t position = map_str_find(map, key);
  
  return position == map_str_end(map) ? NULL : map_str_get(map, position);
}

/* element lookup */

size_t map_str_find(map_str *map, char *key)
{
  size_t mask = map->capacity - 1, h = cfarmhash(key, strlen(key)), i = h & mask, step = 0;

  while (1)
    {
      if (map->keys[i] == MAP_STR_RESERVED_EMPTY)
	return map_str_end(map);

      if (map->keys[i] != MAP_STR_RESERVED_DELETED && strcmp(map->keys[i], key) == 0)
	return i;

      step ++;
      i = (i + step) & mask;
    }
}
 
/* modifiers */

void map_str_insert(map_str *map, char *key, void *value)
{
  size_t position;

  map_str_reserve(map, map->size + 1);
  position = map_str_find_free(map, key);
  if (position == map_str_end(map))
    {
      if (map->release)
	map->release(key, value);
    }
  else
    map_str_put(map, position, key, value);
}

void map_str_erase(map_str *map, char *key)
{
  size_t position;
  
  position = map_str_find(map, key);
  if (position != map_str_end(map))
    {
      if (map->release)
	map->release(map->keys[position], map_str_get(map, position));
      map->keys[position] = MAP_STR_RESERVED_DELETED;
      map->deleted ++;
      map->size --;
    }
}

void map_str_clear(map_str *map)
{
  size_t position;

  if (map->release)
    for (position = map_str_begin(map); 
	 position != map_str_end(map); 
	 position = map_str_next(map, position))
      map->release(map->keys[position], map_str_get(map, position));

  if (map->keys)
    {
      free(map->keys);
      map->keys = NULL;
    }

  if (map->values)
    {
      free(map->values);
      map->values = NULL;
    }
  
  map->size = 0;
  map->deleted = 0;
  map->capacity = 0;
  map->watermark = 0;
}

/* buckets */

size_t map_str_bucket_count(map_str *map)
{
  return map->capacity;
}

/* hash policy */

void map_str_max_load_factor(map_str *map, double max_load_factor)
{
  map->max_load_factor = max_load_factor;
}

void map_str_rehash(map_str *map, size_t capacity_requested)
{
  map_str map_old;
  size_t capacity, position;

  if (map_str_roundup_size(capacity_requested + map->deleted) > map->capacity)
    {
      map_old = *map;
      capacity = map_str_roundup_size(capacity_requested);
      map->keys = (char **) calloc(capacity, sizeof(*map->keys));
      map->values = malloc(capacity * map->value_size);

      map->size = 0;
      map->deleted = 0;
      map->capacity = capacity;
      map->watermark = capacity * map->max_load_factor; 
      
      if (map_old.size)
	for (position = map_str_begin(&map_old); 
	     position != map_str_end(&map_old); 
	     position = map_str_next(&map_old, position))
	  map_str_insert(map, map_old.keys[position], map_str_get(&map_old, position));
      
      if (map_old.keys)
	free(map_old.keys);

      if (map_old.values)
	free(map_old.values);
    }
}

void map_str_reserve(map_str *map, size_t size)
{
  if (size + map->deleted > map->watermark)
    map_str_rehash(map, size / map->max_load_factor);
}

/* iterators */

size_t map_str_begin(map_str *map)
{
  return map_str_next_inclusive(map, 0);
}

size_t map_str_next(map_str *map, size_t position)
{
  return map_str_next_inclusive(map, position + 1);
}

size_t map_str_end(map_str *map)
{
  return map->capacity;
}

/* internals */

size_t map_str_find_free(map_str *map, char *key)
{
  size_t mask = map->capacity - 1, h = cfarmhash(key, strlen(key)), i = h & mask, step = 0;

  while (1)
    {
      if (map->keys[i] <= MAP_STR_RESERVED_DELETED)
	return i;

      if (strcmp(map->keys[i], key) == 0)
	return map_str_end(map);
      
      step ++;
      i = (i + step) & mask;
    }
}

size_t map_str_next_inclusive(map_str *map, size_t position)
{
  while (position < map->capacity)
    {
      if (map->keys[position] > MAP_STR_RESERVED_DELETED)
	break;
      position ++;
    }
  
  return position;
}

size_t map_str_roundup_size(size_t size)
{
  size --;
  size |= size >> 1;
  size |= size >> 2;
  size |= size >> 4;
  size |= size >> 8;
  size |= size >> 16;
  size |= size >> 32;
  size ++;

  return size;
}

void *map_str_data_offset(void *data, size_t offset)
{
  return (char *) data + offset;
}
