#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/param.h>

#include "cfarmhash.h"
#include "maps.h"

/* allocators */

maps *maps_new(size_t value_size)
{
  maps *map;

  map = malloc(sizeof *map);
  maps_init(map, value_size);

  return map;
}

void maps_init(maps *map, size_t value_size)
{
  memset(map, 0, sizeof *map);
  map->value_size = value_size;
  maps_max_load_factor(map, MAPS_MAX_LOAD_FACTOR);
}

void maps_release(maps *map, void (*release)(char *, void *))
{
  map->release = release;
}

void maps_free(maps *map)
{
  maps_clear(map);
  free(map);
}

/* capacity */

size_t maps_size(maps *map)
{
  return map->size;
}

/* Element access */

void *maps_get(maps *map, size_t position)
{
  return maps_data_offset(map->values, position * map->value_size);
}

void maps_put(maps *map, size_t position, char *key, void *value)
{
  if (map->keys[position] == MAPS_RESERVED_DELETED)
    map->deleted --;
  map->keys[position] = key;
  memcpy(maps_get(map, position), value, map->value_size);
  map->size ++;
}

void *maps_at(maps *map, char *key)
{
  size_t position = maps_find(map, key);

  return position == maps_end(map) ? NULL : maps_get(map, position);
}

/* element lookup */

size_t maps_find(maps *map, char *key)
{
  size_t mask = map->capacity - 1, h = cfarmhash(key, strlen(key)), i = h & mask, step = 0;

  while (1)
    {
      if (map->keys[i] == MAPS_RESERVED_EMPTY)
        return maps_end(map);

      if (map->keys[i] != MAPS_RESERVED_DELETED && strcmp(map->keys[i], key) == 0)
        return i;

      step ++;
      i = (i + step) & mask;
    }
}

/* modifiers */

void maps_insert(maps *map, char *key, void *value)
{
  size_t position;

  maps_reserve(map, map->size + 1);
  position = maps_find_free(map, key);
  if (position == maps_end(map))
    {
      if (map->release)
        map->release(key, value);
    }
  else
    maps_put(map, position, key, value);
}

void maps_erase(maps *map, char *key)
{
  size_t position;

  position = maps_find(map, key);
  if (position != maps_end(map))
    {
      if (map->release)
        map->release(map->keys[position], maps_get(map, position));
      map->keys[position] = MAPS_RESERVED_DELETED;
      map->deleted ++;
      map->size --;
    }
}

void maps_clear(maps *map)
{
  size_t position;

  if (map->release)
    for (position = maps_begin(map);
         position != maps_end(map);
         position = maps_next(map, position))
      map->release(map->keys[position], maps_get(map, position));

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

size_t maps_bucket_count(maps *map)
{
  return map->capacity;
}

/* hash policy */

void maps_max_load_factor(maps *map, double max_load_factor)
{
  map->max_load_factor = max_load_factor;
}

void maps_rehash(maps *map, size_t capacity_requested)
{
  maps map_old;
  size_t capacity, position;

  if (maps_roundup_size(capacity_requested + map->deleted) > map->capacity)
    {
      map_old = *map;
      capacity = maps_roundup_size(capacity_requested);
      map->keys = (char **) calloc(capacity, sizeof(*map->keys));
      map->values = malloc(capacity * map->value_size);

      map->size = 0;
      map->deleted = 0;
      map->capacity = capacity;
      map->watermark = capacity * map->max_load_factor;

      if (map_old.size)
        for (position = maps_begin(&map_old);
             position != maps_end(&map_old);
             position = maps_next(&map_old, position))
          maps_insert(map, map_old.keys[position], maps_get(&map_old, position));

      if (map_old.keys)
        free(map_old.keys);

      if (map_old.values)
        free(map_old.values);
    }
}

void maps_reserve(maps *map, size_t size)
{
  if (size + map->deleted > map->watermark)
    maps_rehash(map, size / map->max_load_factor);
}

/* iterators */

size_t maps_begin(maps *map)
{
  return maps_next_inclusive(map, 0);
}

size_t maps_next(maps *map, size_t position)
{
  return maps_next_inclusive(map, position + 1);
}

size_t maps_end(maps *map)
{
  return map->capacity;
}

/* internals */

size_t maps_find_free(maps *map, char *key)
{
  size_t mask = map->capacity - 1, h = cfarmhash(key, strlen(key)), i = h & mask, step = 0;

  while (1)
    {
      if (map->keys[i] <= MAPS_RESERVED_DELETED)
        return i;

      if (strcmp(map->keys[i], key) == 0)
        return maps_end(map);

      step ++;
      i = (i + step) & mask;
    }
}

size_t maps_next_inclusive(maps *map, size_t position)
{
  while (position < map->capacity)
    {
      if (map->keys[position] > MAPS_RESERVED_DELETED)
        break;
      position ++;
    }

  return position;
}

size_t maps_roundup_size(size_t size)
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

void *maps_data_offset(void *data, size_t offset)
{
  return (char *) data + offset;
}
