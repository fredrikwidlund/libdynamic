#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <err.h>

#include <sys/epoll.h>
#include <sys/timerfd.h>

#include <dynamic.h>

struct state
{
  core core;
  pool pool;
  int timer;
  _Atomic size_t jobs;
};

void async(void *state)
{
  usleep(10000);
  ((struct state *) state)->jobs++;
}

core_status collect(core_event *event)
{
  struct state *state = event->state;
  void *result;

  do
    result = pool_collect(&state->pool, POOL_DONTWAIT);
  while (result);

  return CORE_OK;
}

core_status timeout(core_event *event)
{
  struct state *state = event->state;
  core_counters *counters;
  uint64_t exp;
  ssize_t n;
  int i;

  while (1)
  {
    n = read(state->timer, &exp, sizeof exp);
    if (n == -1 && errno == EAGAIN)
      break;
    if (n != sizeof exp)
      err(1, "read");

    counters = core_get_counters(&state->core);
    (void) printf("[timer %lu, jobs %lu]\n", exp, state->jobs);
    (void) printf("[stats polls %lu, events %lu, awake %lu, total %lu, usage %f\n",
                  counters->polls, counters->events, counters->awake, counters->awake + counters->sleep,
                  (double) counters->awake / (double) (counters->awake + counters->sleep));
    core_clear_counters(&state->core);
  }

  for (i = 0; i < 1000; i++)
    pool_enqueue(&state->pool, async, state);
  return CORE_OK;
}

int main()
{
  struct state state = {0};

  core_construct(&state.core);
  pool_construct(&state.pool);

  state.timer = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK | TFD_CLOEXEC);
  timerfd_settime(state.timer, 0, (struct itimerspec[]) {{{1, 0}, {1, 0}}}, NULL);
  core_add(&state.core, timeout, &state, state.timer, EPOLLIN | EPOLLET);
  core_add(&state.core, collect, &state, pool_fd(&state.pool), EPOLLIN | EPOLLET);

  core_loop(&state.core);

  close(state.timer);
  pool_destruct(&state.pool);
  core_destruct(&state.core);
}
