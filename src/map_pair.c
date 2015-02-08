#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/param.h>

#include "map_pair.h"

/* allocators */

map_pair *map_pair_new()
{
  map_pair *map;
  
  map = (map_pair *) malloc(sizeof *map);
  map_pair_init(map);
  
  return map;
}

void map_pair_init(map_pair *map)
{
  memset(map, 0, sizeof *map);
  map_pair_max_load_factor(map, MAP_PAIR_MAX_LOAD_FACTOR);
}

void map_pair_release(map_pair *map, void (*release)(uint64_t, uint64_t))
{
  map->release = release;
}

void map_pair_free(map_pair *map)
{
  map_pair_clear(map);
  free(map);
}

/* capacity */

size_t map_pair_size(map_pair *map)
{
  return map->size;
}

/* Element access */

uint64_t map_pair_get(map_pair *map, size_t position)
{
  return map->values[position];
}

void map_pair_put(map_pair *map, size_t position, uint64_t key, uint64_t value)
{
  if (map->keys[position] == MAP_PAIR_DELETED)
    map->deleted --;
  map->keys[position] = key;
  map->values[position] = value;
  map->size ++;
  }

uint64_t map_pair_at(map_pair *map, uint64_t key)
{
  size_t position = map_pair_find(map, key);
  
  return position == map_pair_end(map) ? MAP_PAIR_EMPTY : map_pair_get(map, position);
}

/* element lookup */

size_t map_pair_find(map_pair *map, uint64_t key)
{
  size_t mask = map->capacity - 1, i = key & mask, step = 0;
  
  while (1)
    {
      if (__likely(map->keys[i] == key))
	return i;
      if (__likely(map->keys[i] == MAP_PAIR_EMPTY))
	return map_pair_end(map);
      step ++;
      i = (i + step) & mask;
    }
}

/* modifiers */

void map_pair_insert(map_pair *map, uint64_t key, uint64_t value)
{
  size_t position;
  
  map_pair_reserve(map, map->size + 1);
  position = map_pair_find_free(map, key);
  if (position == map_pair_end(map))
    {
      if (map->release)
	map->release(key, map_pair_get(map, position));
    }
  else
    map_pair_put(map, position, key, value);
}

void map_pair_erase(map_pair *map, uint64_t key)
{
  size_t position;
  
  position = map_pair_find(map, key);
  if (position != map_pair_end(map))
    {
      if (map->release)
	map->release(key, map_pair_get(map, position));
      map->keys[position] = MAP_PAIR_DELETED;
      map->deleted ++;
      map->size --;      
    }
}

void map_pair_clear(map_pair *map)
{
  size_t position;

  if (map->release)
    for (position = map_pair_begin(map); 
	 position != map_pair_end(map); 
	 position = map_pair_next(map, position))
      map->release(map->keys[position], map_pair_get(map, position));

  free(map->keys);
  map->keys = NULL;

  free(map->values);
  map->values = NULL;
  
  map->size = 0;
  map->deleted = 0;
  map->capacity = 0;
  map->watermark = 0;
}

/* buckets */

size_t map_pair_bucket_count(map_pair *map)
{
  return map->capacity;
}

/* hash policy */

void map_pair_max_load_factor(map_pair *map, double max_load_factor)
{
  map->max_load_factor = max_load_factor;
}

void map_pair_rehash(map_pair *map, size_t capacity_requested)
{
  map_pair map_old;
  size_t capacity, i, position;

  if (map_pair_roundup_size(capacity_requested + map->deleted) > map->capacity)
    {
      map_old = *map;
      capacity = map_pair_roundup_size(capacity_requested);
      map->keys = malloc(capacity * sizeof(*map->keys));
      map->values = malloc(capacity * sizeof(*map->values));
      for (i = 0; i < capacity; i ++)
	map->keys[i] =  MAP_PAIR_EMPTY;
      
      map->size = 0;
      map->deleted = 0;
      map->capacity = capacity;
      map->watermark = capacity * map->max_load_factor; 
      
      if (map_old.size)
	for (position = map_pair_begin(&map_old); 
	     position != map_pair_end(&map_old); 
	     position = map_pair_next(&map_old, position))
	  map_pair_insert(map, map_old.keys[position], map_pair_get(&map_old, position));
      
      free(map_old.keys);
      free(map_old.values);
    }
}

void map_pair_reserve(map_pair *map, size_t size)
{
  if (size + map->deleted > map->watermark)
    map_pair_rehash(map, size / map->max_load_factor);
}

/* iterators */

size_t map_pair_begin(map_pair *map)
{
  return map_pair_next_inclusive(map, 0);
}

size_t map_pair_next(map_pair *map, size_t position)
{
  return map_pair_next_inclusive(map, position + 1);
}

size_t map_pair_end(map_pair *map)
{
  return map->capacity;
}

/* internals */

size_t map_pair_find_free(map_pair *map, uint64_t key)
{
  size_t mask = map->capacity - 1, i = key & mask, step = 0;
  uint64_t test;

  while (1)
    {
      test = map->keys[i];
      if (test == MAP_PAIR_DELETED || test == MAP_PAIR_EMPTY)
	return i;
      if (test == key)
	return map_pair_end(map);
      step ++;
      i = (i + step) & mask;
    }
}

size_t map_pair_next_inclusive(map_pair *map, size_t position)
{
  uint64_t test;
  
  while (position < map->capacity)
    {
      test = map->keys[position];
      if (test != MAP_PAIR_EMPTY && test != MAP_PAIR_DELETED)
	break;
      position ++;
    }
  
  return position;
}

size_t map_pair_roundup_size(size_t size)
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
