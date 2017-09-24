#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <dynamic.h>

#include "stream.h"

static int stream_need(stream *stream, size_t size)
{
  if (size <= stream_size(stream))
    return 1;

  stream_destruct(stream);
  return 0;
}

void stream_construct(stream *stream, buffer *buffer)
{
  stream->buffer = buffer;
  stream->begin = 0;
}

void stream_destruct(stream *stream)
{
  stream->buffer = 0;
  stream->begin = 0;
}

int stream_valid(stream *stream)
{
  return stream->buffer != NULL;
}

size_t stream_size(stream *stream)
{
  return buffer_size(stream->buffer) - stream->begin;
}

uint32_t stream_read32(stream *stream)
{
  uint8_t *bytes = buffer_data(stream->buffer);
  uint32_t value;

  if (!stream_need(stream, 4))
    return 0;

  memcpy(&value, bytes + stream->begin, 4);
  value = be32toh(value);
  stream->begin += 4;

  return value;
}

void stream_write(stream *stream, void *data, size_t size)
{
  if (!stream_valid(stream))
    return;

  buffer_insert(stream->buffer, stream_size(stream), data, size);
}

void stream_write32(stream *stream, uint32_t value)
{
  value = htobe32(value);
  stream_write(stream, &value, 4);
}
