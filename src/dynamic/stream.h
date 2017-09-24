#ifndef STREAM_H_INCLUDED
#define STREAM_H_INCLUDED

typedef struct stream stream;
struct stream
{
  buffer *buffer;
  size_t  begin;
};

void      stream_construct(stream *, buffer *);
void      stream_destruct(stream *);
int       stream_valid(stream *);
size_t    stream_size(stream *);

uint8_t   stream_read8(stream *);
uint16_t  stream_read16(stream *);
uint32_t  stream_read32(stream *);
uint64_t  stream_read64(stream *);
void     *stream_read(stream *, size_t);
uint64_t  stream_read_bits(uint64_t, int, int, int);

void      stream_write8(stream *, uint8_t);
void      stream_write16(stream *, uint16_t);
void      stream_write32(stream *, uint32_t);
void      stream_write64(stream *, uint64_t);
void      stream_write(stream *, void *, size_t);
uint64_t  stream_write_bits(uint64_t, int, int, int);

#endif /* STREAM_H_INCLUDED */
