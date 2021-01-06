#ifndef DYNAMIC_POOL_H_INCLUDED
#define DYNAMIC_POOL_H_INCLUDED

#define POOL_WORKERS_MIN 0
#define POOL_WORKERS_MAX 16

enum
{
 POOL_MESSAGE_JOB,
 POOL_MESSAGE_WORKER
};

enum
{
  POOL_REQUEST,
  POOL_REPLY
};

typedef struct pool_worker  pool_worker;
typedef struct pool_message pool_message;
typedef struct pool         pool;

struct pool_worker
{
  pthread_t       thread;
  int             active;
  int             socket;
};

struct pool_message
{
  size_t          type;
  union
  {
    core_handler *user;
    pool_worker  *worker;
  };
};

struct pool
{
  core           *core;
  size_t          ref;
  size_t          errors;
  int             socket_master;
  int             socket_worker;
  size_t          workers_min;
  size_t          workers_max;
  list            jobs_waiting;
  list            jobs_queued;
  size_t          jobs_count;
  list            workers;
  size_t          workers_count;
  int             active;
};

void    pool_construct(pool *, core *);
void    pool_destruct(pool *);
void    pool_limits(pool *, size_t, size_t);
size_t  pool_errors(pool *);
core_id pool_enqueue(pool *, core_callback *, void *);
void    pool_cancel(pool *, core_id);
void    pool_abort(pool *);

#endif /* DYNAMIC_POOL_H_INCLUDED */
