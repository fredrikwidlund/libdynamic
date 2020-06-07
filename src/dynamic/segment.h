#ifndef SEGMENT_H_INCLUDED
#define SEGMENT_H_INCLUDED

typedef struct segment segment;

struct segment
{
  void   *base;
  size_t  size;
};

segment segment_empty();
segment segment_data(void *, size_t);
segment segment_string(char *);
segment segment_offset(segment , size_t);
int     segment_equal(segment, segment);

#endif /* SEGMENT_H_INCLUDED */
