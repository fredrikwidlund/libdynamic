#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>
#include <setjmp.h>
#include <cmocka.h>

#include "../src/dynamic/list.h"
#include "../src/dynamic/pool.h"

extern int debug_recv;
extern int debug_send;
extern int debug_pthread_create;
extern int debug_socketpair;

void job(void *arg)
{
  _Atomic int *state = arg;

  (*state) ++;
}

void job_failsend(void *arg)
{
  _Atomic int *state = arg;

  debug_send = 1;
  (*state) ++;
}

void core()
{
  pool p;
  int i, state = 0, *statep;

  /* create pool */
  pool_construct(&p);
  pool_limits(&p, 1, 8);

  /* process jobs */
  for (i = 0; i < 1000; i ++)
    pool_enqueue(&p, job, &state);
  while (pool_jobs(&p))
    statep = pool_collect(&p, 0);
  assert_int_equal(state, 1000);
  assert_int_equal(*statep, 1000);

  /* collect when no jobs */
  pool_collect(&p, POOL_DONTWAIT);
  assert_int_equal(state, 1000);

  /* create jobs without collecting */
  for (i = 0; i < 1000; i ++)
    pool_enqueue(&p, job, &state);
  assert_true(pool_fd(&p) >= 0);
  assert_int_equal(pool_error(&p), 0);

  /* destroy pool */
  pool_destruct(&p);
}

void call_failures()
{
  pool p;
  int state = 0;

  /* recv */
  debug_recv = 1;
  pool_construct(&p);
  pool_enqueue(&p, job, &state);
  pool_collect(&p, 0);
  pool_destruct(&p);
  debug_recv = 0;
  assert_int_equal(state, 0);

  /* send */
  pool_construct(&p);
  debug_send = 1;
  errno = 0;
  pool_enqueue(&p, job, &state);
  pool_collect(&p, POOL_DONTWAIT);
  pool_destruct(&p);
  debug_send = 0;
  assert_int_equal(state, 0);

  /* send in thread */
  pool_construct(&p);
  pool_enqueue(&p, job_failsend, &state);
  usleep(100000);
  pool_collect(&p, POOL_DONTWAIT);
  pool_destruct(&p);
  debug_send = 0;
  assert_int_equal(state, 1);

  /* pthread_create */
  debug_pthread_create = 1;
  pool_construct(&p);
  pool_enqueue(&p, job, &state);
  pool_collect(&p, POOL_DONTWAIT);
  pool_destruct(&p);
  debug_pthread_create = 0;
  assert_int_equal(state, 1);

  /* socketpair */
  debug_socketpair = 1;
  pool_construct(&p);
  pool_destruct(&p);
  debug_socketpair = 0;
  assert_int_equal(state, 1);
}

int main()
{
  const struct CMUnitTest tests[] =
    {
     cmocka_unit_test(core),
     cmocka_unit_test(call_failures),
    };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
