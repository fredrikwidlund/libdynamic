#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "dynamic.h"

segment segment_data(void *base, size_t size)
{
  return (segment) {.base = base, .size = size};
}

segment segment_empty(void)
{
  return segment_data(NULL, 0);
}

segment segment_string(char *string)
{
  return segment_data(string, strlen(string));
}

segment segment_offset(segment s, size_t offset)
{
  return (segment) {.base = (char *) s.base + offset, s.size - offset};
}

int segment_equal(segment s1, segment s2)
{
  return s1.size == s2.size && memcmp(s1.base, s2.base, s1.size) == 0;
}
