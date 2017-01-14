#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <cmocka.h>

int debug_out_of_memory = 0;
int debug_abort = 0;

void *__real_malloc(size_t);
void *__wrap_malloc(size_t size)
{
  return debug_out_of_memory ? NULL : __real_malloc(size);
}

void *__real_calloc(size_t, size_t);
void *__wrap_calloc(size_t n, size_t size)
{
  return debug_out_of_memory ? NULL : __real_calloc(n, size);
}

void *__real_realloc(void *, size_t);
void *__wrap_realloc(void *p, size_t size)
{
  return debug_out_of_memory ? NULL : __real_realloc(p, size);
}

void *__real_aligned_alloc(size_t, size_t);
void *__wrap_aligned_alloc(size_t align, size_t size)
{
  return debug_out_of_memory ? NULL : __real_aligned_alloc(align, size);

}

void __real_abort(void);
void __wrap_abort(void)
{
  if (debug_abort)
    mock_assert(0, "", "", 0);
  else
    __real_abort();
}
