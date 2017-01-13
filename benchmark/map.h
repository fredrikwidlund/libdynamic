#ifndef BENCHMARKS_MAP_H_INCLUDED
#define BENCHMARKS_MAP_H_INCLUDED

typedef struct map_metric map_metric;
struct map_metric
{
  char    *name;
  void   (*measure)(map_metric *, uint32_t *, size_t);
  double   insert;
  double   at;
};

uint64_t ntime(void);

#endif /* BENCHMARKS_MAP_H_INCLUDED */
