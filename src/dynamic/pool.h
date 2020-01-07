#ifndef POOL_H_INCLUDED
#define POOL_H_INCLUDED

#define POOL_WORKERS_MIN 1
#define POOL_WORKERS_MAX 16

enum pool_message_types
{
 POOL_MESSAGE_JOB,
 POOL_MESSAGE_CONTROL
};

enum pool_flags
{
  POOL_DONTWAIT = 0x01
};

typedef void                pool_callback(void *);
typedef struct pool_worker  pool_worker;
typedef struct pool_job     pool_job;
typedef struct pool_control pool_control;
typedef struct pool_message pool_message;
typedef struct pool         pool;

struct pool_worker
{
  pthread_t          thread;
  int                socket;
};

struct pool_job
{
  pool_callback     *callback;
  void              *state;
};

struct pool_control
{
  pool_worker       *worker;
};

struct pool_message
{
  int                type;
  union
  {
    pool_job         job;
    pool_control     control;
  };
};

struct pool
{
  int                socket;
  int                error;

  size_t             workers_min;
  size_t             workers_max;
  int                workers_socket;
  list               workers;
  size_t             workers_count;

  list               messages_queued;
  list               messages_transit;

  size_t             jobs_count;
};

void   pool_construct(pool *);
void   pool_destruct(pool *);
void   pool_limits(pool *, size_t, size_t);
size_t pool_jobs(pool *);
int    pool_fd(pool *);
int    pool_error(pool *);
void   pool_enqueue(pool *, pool_callback *, void *);
void  *pool_collect(pool *, int);

#endif /* POOL_H_INCLUDED */
