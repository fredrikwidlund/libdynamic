#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <sys/types.h>

#include "list.h"
#include "pool.h"

static void *pool_thread(void *arg)
{
  pool_worker *worker = arg;
  pool_message *message;
  ssize_t n;
  int active = 1;

  while (active)
  {
    n = recv(worker->socket, &message, sizeof message, 0);
    if (n == -1)
      break;

    if (message->type == POOL_MESSAGE_JOB)
    {
      message->job.callback(message->job.state);
    }
    else
    {
      message->control.worker = arg;
      active = 0;
    }

    n = send(worker->socket, &message, sizeof message, 0);
    if (n == -1)
      break;
  }

  return NULL;
}

static void pool_flush(pool *pool)
{
  pool_message *message;
  ssize_t n;

  while (!list_empty(&pool->messages_queued))
  {
    message = list_front(&pool->messages_queued);
    n = send(pool->socket, &message, sizeof message, MSG_DONTWAIT);
    if (n == -1)
    {
      if (errno != EAGAIN)
        pool->errors++;
      break;
    }
    list_splice(list_front(&pool->messages_transit), message);
  }
}

static void pool_maintain(pool *pool)
{
  pool_worker *worker;
  pool_message *message;
  size_t i, n;
  int e;

  n = MIN(MAX(pool->jobs_count, pool->workers_min), pool->workers_max);

  for (i = pool->workers_count; i < n; i++)
  {
    worker = list_push_back(&pool->workers, NULL, sizeof *worker);
    worker->socket = pool->workers_socket;
    e = pthread_create(&worker->thread, NULL, pool_thread, worker);
    if (e == -1)
    {
      list_erase(worker, NULL);
      pool->errors++;
      return;
    }
    pool->workers_count++;
  }

  for (; n < pool->workers_count; n++)
  {
    message = list_push_front(&pool->messages_queued, NULL, sizeof *message);
    message->type = POOL_MESSAGE_CONTROL;
    pool->workers_count--;
  }

  pool_flush(pool);
}

void pool_construct(pool *pool)
{
  int e, fd[2] = {-1, -1};

  *pool = (struct pool) {0};

  list_construct(&pool->workers);
  list_construct(&pool->messages_queued);
  list_construct(&pool->messages_transit);

  e = socketpair(AF_UNIX, SOCK_STREAM, PF_UNSPEC, fd);
  if (e == -1)
    pool->errors++;

  pool->socket = fd[0];
  pool->workers_socket = fd[1];
  pool_limits(pool, POOL_WORKERS_MIN, POOL_WORKERS_MAX);
}

void pool_destruct(pool *pool)
{
  pool_worker *worker;
  void *state;

  do
    state = pool_collect(pool, POOL_DONTWAIT);
  while (state);

  list_foreach(&pool->workers, worker)
  {
    pthread_cancel(worker->thread);
    pthread_join(worker->thread, NULL);
  }

  list_destruct(&pool->workers, NULL);
  list_destruct(&pool->messages_queued, NULL);
  list_destruct(&pool->messages_transit, NULL);

  if (pool->socket >= 0)
    close(pool->socket);
  if (pool->workers_socket >= 0)
    close(pool->workers_socket);
}

void pool_limits(pool *pool, size_t min, size_t max)
{
  pool->workers_min = min;
  pool->workers_max = max;
  pool_maintain(pool);
}

size_t pool_jobs(pool *pool)
{
  return pool->jobs_count;
}

int pool_fd(pool *pool)
{
  return pool->socket;
}

int pool_error(pool *pool)
{
  return pool->errors != 0;
}

void pool_enqueue(pool *pool, pool_callback *callback, void *state)
{
  pool_message *message;

  message = list_push_back(&pool->messages_queued, NULL, sizeof *message);
  message->type = POOL_MESSAGE_JOB;
  message->job.callback = callback;
  message->job.state = state;
  pool->jobs_count++;
  pool_maintain(pool);
}

void *pool_collect(pool *pool, int flags)
{
  pool_message *message;
  ssize_t n;
  void *state;

  pool_maintain(pool);
  while (1)
  {
    n = recv(pool->socket, &message, sizeof message, flags & POOL_DONTWAIT ? MSG_DONTWAIT : 0);
    if (n == -1)
    {
      if (errno != EAGAIN)
        pool->errors++;
      return NULL;
    }

    if (message->type == POOL_MESSAGE_JOB)
    {
      pool->jobs_count--;
      state = message->job.state;
      list_erase(message, NULL);
      return state;
    }
    else
    {
      pthread_cancel(message->control.worker->thread);
      pthread_detach(message->control.worker->thread);
      list_erase(message->control.worker, NULL);
      list_erase(message, NULL);
    }
  }
}
