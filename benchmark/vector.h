#ifndef BENCHMARKS_VECTOR_H_INCLUDED
#define BENCHMARKS_VECTOR_H_INCLUDED

typedef struct vector_metric vector_metric;
struct vector_metric
{
  char    *name;
  void   (*measure)(vector_metric *, size_t);
  double   insert;
};

uint64_t ntime(void);

#endif /* BENCHMARKS_VECTOR_H_INCLUDED */
