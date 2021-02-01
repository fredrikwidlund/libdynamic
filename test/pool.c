#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <errno.h>
#include <setjmp.h>

#include <cmocka.h>

#include "dynamic.h"

extern int debug_abort;
extern int debug_recv;
extern int debug_send;
extern int debug_socketpair;

static core_status job(core_event *event)
{
  _Atomic int *state = event->state;

  if (event->type == POOL_REQUEST)
    (*state)++;
  return CORE_OK;
}

static core_status job_failsend(core_event *event)
{
  _Atomic int *state = event->state;

  if (event->type == POOL_REQUEST)
  {
    debug_send = 1;
    (*state)++;
  }
  return CORE_OK;
}

static core_status cancel(core_event *event)
{
  (void) event;
  core_abort(NULL);
  return CORE_OK;
}

static core_status broken_pipe(core_event *event)
{
  pool *pool = event->state;

  close(pool->socket_worker);
  core_abort(NULL);
  return CORE_OK;
}

void basic(__attribute__((unused)) void **ununsed)
{
  pool p = {0};
  int i, state = 0;

  // create pool
  core_construct(NULL);

  // abort on non active pool
  pool_construct(&p, NULL);
  pool_abort(&p);
  pool_destruct(&p);

  // create default pool
  pool_construct(NULL, NULL);
  assert_int_equal(pool_errors(NULL), 0);
  pool_destruct(NULL);

  // create local pool
  pool_construct(&p, NULL);

  // reduce workers
  pool_limits(&p, 32, 32);
  pool_enqueue(&p, job, &state);
  pool_limits(&p, 1, 16);
  pool_enqueue(&p, job, &state);

  // process jobs
  for (i = 0; i < 100; i++)
    pool_enqueue(&p, job, &state);
  core_loop(NULL);
  assert_int_equal(state, 102);

  // create jobs with running core loop
  for (i = 0; i < 100; i++)
    pool_enqueue(&p, job, &state);
  assert_int_equal(pool_errors(&p), 0);

  // destroy pool
  pool_destruct(&p);

  // run core loop with pool removed
  core_loop(NULL);
  core_destruct(NULL);

  // abort
  signal(SIGTERM, SIG_IGN);
  core_construct(NULL);
  pool_construct(&p, NULL);
  pool_enqueue(&p, job, &state);
  core_loop(NULL);
  pool_abort(&p);
  pool_destruct(&p);
  core_destruct(NULL);

  // abort default instance
  signal(SIGTERM, SIG_IGN);
  core_construct(NULL);
  pool_construct(NULL, NULL);
  pool_enqueue(NULL, job, &state);
  core_loop(NULL);
  pool_abort(NULL);
  pool_destruct(NULL);
  core_destruct(NULL);
}

void fails(__attribute__((unused)) void **unused)
{
  pool p = {0};
  core_id id;
  int state = 0;

  core_construct(NULL);

  // send in thread
  pool_construct(&p, NULL);
  pool_enqueue(&p, job_failsend, &state);
  usleep(100000);
  pool_destruct(&p);
  debug_send = 0;
  assert_int_equal(state, 1);

  // recv in thread
  state = 0;
  debug_recv = 1;
  pool_construct(&p, NULL);
  pool_enqueue(&p, job, &state);
  usleep(100000);
  pool_destruct(&p);
  debug_recv = 0;
  assert_int_equal(state, 0);

  // send in main job
  pool_construct(&p, NULL);
  errno = 0;
  debug_send = 1;
  pool_enqueue(&p, job, &state);
  debug_send = 0;
  assert_int_equal(pool_errors(&p), 1);
  pool_destruct(&p);

  // send in main reduce
  pool_construct(&p, NULL);
  pool_limits(&p, 2, 2);
  pool_enqueue(&p, job, &state);
  pool_limits(&p, 0, 1);
  errno = 0;
  debug_send = 1;
  pool_enqueue(&p, job, &state);
  debug_send = 0;
  assert_int_equal(pool_errors(&p), 2);
  pool_destruct(&p);

  // send in main reduce default pool
  pool_construct(NULL, NULL);
  pool_limits(NULL, 2, 2);
  pool_enqueue(NULL, job, &state);
  pool_limits(NULL, 0, 1);
  errno = 0;
  debug_send = 1;
  pool_enqueue(NULL, job, &state);
  debug_send = 0;
  assert_int_equal(pool_errors(NULL), 2);
  pool_destruct(NULL);

  // cancel
  pool_construct(&p, NULL);
  id = pool_enqueue(&p, job, &state);
  pool_cancel(&p, id);
  core_loop(NULL);
  pool_destruct(&p);

  // receive in main
  pool_construct(&p, NULL);
  pool_enqueue(&p, job, &state);
  core_next(NULL, cancel, NULL);
  usleep(100000);
  debug_recv = 1;
  errno = 0;
  core_loop(NULL);
  debug_recv = 0;
  pool_destruct(&p);
  core_destruct(NULL);

  // receive events
  core_construct(NULL);
  pool_construct(&p, NULL);
  pool_enqueue(&p, job, &state);
  core_next(NULL, broken_pipe, &p);
  core_loop(NULL);
  pool_destruct(&p);
  core_destruct(NULL);
  assert_int_equal(pool_errors(&p), 1);

  // multiple refs
  pool_construct(&p, NULL);
  pool_construct(&p, NULL);
  pool_destruct(&p);
  pool_destruct(&p);

  // socketpair
  core_construct(NULL);
  debug_socketpair = 1;
  pool_construct(&p, NULL);
  pool_destruct(&p);
  debug_socketpair = 0;
  assert_int_equal(pool_errors(&p), 1);
  core_destruct(NULL);
}

int main()
{
  const struct CMUnitTest tests[] =
      {
          cmocka_unit_test(basic),
          cmocka_unit_test(fails),
      };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
