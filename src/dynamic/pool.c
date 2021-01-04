#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>
#include <errno.h>
#include <assert.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/epoll.h>

#include "list.h"
#include "buffer.h"
#include "vector.h"
#include "core.h"
#include "pool.h"

static __thread pool pool_default = {0};
static __thread int pool_activated = 0;

static void pool_update(pool *);

static pool *pool_get(pool *pool)
{
  return pool ? pool : &pool_default;
}

static void *pool_thread(void *arg)
{
  pool_worker *worker = arg;
  pool_message message;
  ssize_t n;
  int active = 1;

  while (active)
  {
    n = recv(worker->socket, &message, sizeof message, 0);
    if (n == -1)
      break;

    switch (message.type)
    {
    case POOL_MESSAGE_JOB:
      if (message.user->callback)
        (void) core_dispatch(message.user, POOL_REQUEST, 0);
      break;
    default:
      active = 0;
      message.type = POOL_MESSAGE_WORKER;
      message.worker = worker;
      break;
    }

    n = send(worker->socket, &message, sizeof message, 0);
    if (n == -1)
      break;
  }

  return NULL;
}

static core_status pool_receive(core_event *event)
{
  pool *pool = event->state;
  pool_message message;
  ssize_t n;

  if (event->data != EPOLLIN)
  {
    pool->errors++;
    return CORE_ABORT;
  }

  while (1)
  {
    n = recv(pool->socket_master, &message, sizeof message, MSG_DONTWAIT);
    if (n == -1)
    {
      if (errno != EAGAIN)
        pool->errors++;
      break;
    }

    switch (message.type)
    {
    case POOL_MESSAGE_JOB:
      if (message.user->callback)
        (void) core_dispatch(message.user, POOL_REPLY, 0);
      list_erase(message.user, NULL);
      pool->jobs_count--;
      break;
    default:
    case POOL_MESSAGE_WORKER:
      pthread_cancel(message.worker->thread);
      pthread_join(message.worker->thread, NULL);
      list_erase(message.worker, NULL);
      break;
    }
  }

  pool_update(pool);
  return CORE_OK;
}

static int pool_send(pool *pool, pool_message *message)
{
  ssize_t n;

  n = send(pool->socket_master, message, sizeof *message, MSG_DONTWAIT);
  if (n == -1)
  {
    pool->errors += errno != EAGAIN;
    return -1;
  }
  return 0;
}

static void pool_update(pool *pool)
{
  pool_worker *worker;
  core_handler *user;
  size_t workers_goal;
  int e;

  workers_goal = MIN(MAX(pool->jobs_count, pool->workers_min), pool->workers_max);

  while (pool->workers_count < workers_goal)
  {
    worker = list_push_back(&pool->workers, NULL, sizeof *worker);
    worker->socket = pool->socket_worker;
    e = pthread_create(&worker->thread, NULL, pool_thread, worker);
    pool->errors += e == -1;
    pool->workers_count++;
  }

  while (pool->workers_count > workers_goal)
  {
    e = pool_send(pool, (pool_message[]) {{.type = POOL_MESSAGE_WORKER}});
    if (e == -1)
      break;
    pool->workers_count--;
  }

  while (!list_empty(&pool->jobs_queued))
  {
    user = list_front(&pool->jobs_queued);
    e = pool_send(pool, (pool_message[]) {{.type = POOL_MESSAGE_JOB, .user = user}});
    if (e == -1)
      break;
    list_splice(list_front(&pool->jobs_waiting), user);
  }

  if (pool->jobs_count && !pool->active)
  {
    core_add(pool->core, pool_receive, pool, pool->socket_master, EPOLLIN | EPOLLET);
    pool->active = 1;
  }

  if (pool->jobs_count == 0)
  {
    core_delete(pool->core, pool->socket_master);
    pool->active = 0;
  }
}

void pool_construct(pool *pool, core *core)
{
  int e, fd[2];

  pool = pool_get(pool);
  if (pool->ref == 0)
  {
    pool->core = core;
    pool->errors = 0;
    list_construct(&pool->jobs_waiting);
    list_construct(&pool->jobs_queued);
    pool->jobs_count = 0;

    list_construct(&pool->workers);
    pool->workers_count = 0;

    e = socketpair(AF_UNIX, SOCK_SEQPACKET, PF_UNSPEC, fd);
    pool->errors += e == -1;
    pool->socket_master = e == 0 ? fd[0] : -1;
    pool->socket_worker = e == 0 ? fd[1] : -1;
    pool_limits(pool, POOL_WORKERS_MIN, POOL_WORKERS_MAX);
  }
  pool->ref++;
}

void pool_destruct(pool *pool)
{
  pool_worker *worker;

  pool = pool_get(pool);
  pool->ref--;
  if (pool->ref > 0)
    return;

  if (pool_activated)
  {
    list_foreach(&pool->workers, worker)
    {
      pthread_cancel(worker->thread);
      pthread_join(worker->thread, NULL);
    }
  }
  list_destruct(&pool->workers, NULL);
  list_destruct(&pool->jobs_queued, NULL);
  list_destruct(&pool->jobs_waiting, NULL);
  pool->jobs_count = 0;

  if (pool->active)
  {
    core_delete(pool->core, pool->socket_master);
    pool->active = 0;
  }

  if (pool->socket_master >= 0)
  {
    close(pool->socket_master);
    pool->socket_master = -1;
  }

  if (pool->socket_worker >= 0)
  {
    close(pool->socket_worker);
    pool->socket_worker = -1;
  }
}

void pool_limits(pool *pool, size_t min, size_t max)
{
  pool = pool_get(pool);
  pool->workers_min = min;
  pool->workers_max = max;
}

size_t pool_errors(pool *pool)
{
  pool = pool_get(pool);
  return pool->errors;
}

core_id pool_enqueue(pool *pool, core_callback *callback, void *state)
{
  core_handler *job;

  pool = pool_get(pool);
  pool_activated = 1;
  job = list_push_back(&pool->jobs_queued, NULL, sizeof *job);
  *job = (core_handler) {.callback = callback, .state = state};
  pool->jobs_count++;

  pool_update(pool);

  return (core_id) job;
}

void pool_cancel(pool *pool, core_id id)
{
  (void) pool;
  *((core_handler *) id) = (core_handler) {0};
}
