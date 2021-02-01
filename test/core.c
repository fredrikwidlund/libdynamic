#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <setjmp.h>
#include <signal.h>
#include <netdb.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/queue.h>

#include <cmocka.h>

#include "dynamic.h"

extern int debug_epoll_create1;
extern int debug_epoll_ctl;
extern int debug_epoll_wait;

static core_status next(core_event *event)
{
  int *n = event->state;

  (*n)++;
  return CORE_OK;
}

static core_status in(core_event *event)
{
  int *fd = event->state;

  core_delete(NULL, *fd);
  return CORE_ABORT;
}

static void basic(__attribute__((unused)) void **state)
{
  core c = {0};
  int n, id, e, p[2];

  /* construct and destruct local instance */
  core_construct(&c);
  core_loop(&c);
  core_destruct(&c);

  /* construct and destruct */
  core_construct(NULL);
  core_construct(NULL);
  core_destruct(NULL);
  core_destruct(NULL);

  /* run empty core */
  core_construct(NULL);
  core_loop(NULL);
  core_destruct(NULL);

  /* add, modify and remove fd */
  core_construct(NULL);
  core_add(NULL, NULL, NULL, 0, 0);
  core_modify(NULL, 0, EPOLLIN);
  core_delete(NULL, 0);
  core_loop(NULL);
  core_destruct(NULL);

    /* add, modify and remove fd with given instance */
  core_construct(&c);
  core_add(&c, NULL, NULL, 0, 0);
  core_modify(&c, 0, EPOLLIN);
  core_delete(&c, 0);
  core_loop(&c);
  core_destruct(&c);

  /* poll event */
  e = pipe(p);
  assert_int_equal(e, 0);
  e = write(p[1], ".", 1);
  assert_int_equal(e, 1);
  core_construct(NULL);
  core_add(NULL, in, &p[0], p[0], EPOLLIN);
  core_loop(NULL);
  core_destruct(NULL);

  /* next, cancel */
  n = 0;
  core_construct(NULL);
  core_next(NULL, next, &n);
  id = core_next(NULL, next, &n);
  core_cancel(NULL, id);
  core_cancel(NULL, 0);
  core_loop(NULL);
  core_destruct(NULL);
  assert_int_equal(n, 1);

  /* next, cancel with given instance */
  n = 0;
  core_construct(&c);
  core_next(&c, next, &n);
  id = core_next(&c, next, &n);
  core_cancel(&c, id);
  core_cancel(&c, 0);
  core_loop(&c);
  core_destruct(&c);
  assert_int_equal(n, 1);

  /* time and counters */
  core_construct(NULL);
  core_now(NULL);
  core_now(NULL);
  core_get_counters(NULL);
  core_clear_counters(NULL);
  core_destruct(NULL);

  core_construct(&c);
  core_now(&c);
  core_now(&c);
  core_get_counters(&c);
  core_clear_counters(&c);
  core_destruct(&c);

  /* abort */
  core_construct(NULL);
  core_abort(NULL);
  core_loop(NULL);
  core_destruct(NULL);

  /* abort instance */
  core_construct(&c);
  core_abort(&c);
  core_loop(&c);
  assert_int_equal(core_errors(&c), 0);
  core_destruct(&c);
}

static void error(__attribute__((unused)) void **state)
{
  /* epoll_create1 error */
  debug_epoll_create1 = 1;
  core_construct(NULL);
  assert_int_equal(core_errors(NULL), 1);
  core_destruct(NULL);
  assert_int_equal(core_errors(NULL), 0);
  debug_epoll_create1 = 0;

  /* epoll_ctl error */
  debug_epoll_ctl = 1;
  core_construct(NULL);
  core_add(NULL, NULL, NULL, 0, 0);
  core_modify(NULL, 0, 0);
  core_delete(NULL, 0);
  assert_int_equal(core_errors(NULL), 3);
  core_destruct(NULL);
  debug_epoll_ctl = 0;

  /* epoll_wait */
  debug_epoll_wait = 1;
  core_construct(NULL);
  core_add(NULL, NULL, NULL, 0, 0);
  core_loop(NULL);
  assert_int_equal(core_errors(NULL), 1);
  core_destruct(NULL);
  debug_epoll_wait = 0;
}

int main()
{
  int e;

  const struct CMUnitTest tests[] =
      {
          cmocka_unit_test(basic),
          cmocka_unit_test(error)};

  e = cmocka_run_group_tests(tests, NULL, NULL);
  (void) close(0);
  (void) close(1);
  (void) close(2);
  return e;
}
