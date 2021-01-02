#ifndef DYNAMIC_CORE_H_INCLUDED
#define DYNAMIC_CORE_H_INCLUDED

#define CORE_MAX_EVENTS 16

enum core_status
{
  CORE_OK    = 0,
  CORE_ABORT = -1
};

typedef uintptr_t            core_id;
typedef enum core_status     core_status;
typedef struct core_event    core_event;
typedef core_status          core_callback(core_event *);
typedef struct core_handler  core_handler;
typedef struct core_counters core_counters;
typedef struct core          core;

struct core_event
{
  void          *state;
  int            type;
  uintptr_t      data;
};

struct core_handler
{
  core_callback *callback;
  void          *state;
};

struct core_counters
{
  uint64_t       awake;
  uint64_t       sleep;
  uint64_t       polls;
  uint64_t       events;
};

struct core
{
  size_t         ref;
  int            fd;
  int            active;
  size_t         errors;
  vector         handlers;
  size_t         handlers_active;
  vector         next;
  uint64_t       time;
  core_counters  counters;
};

void           core_construct(core *);
void           core_destruct(core *);
void           core_abort(core *);
core_status    core_dispatch(core_handler *, int, uintptr_t);
void           core_loop(core *);
void           core_add(core *, core_callback *, void *, int, int);
void           core_modify(core *, int, int);
void           core_delete(core *, int);
core_id        core_next(core *, core_callback *, void *);
void           core_cancel(core *, core_id);
size_t         core_errors(core *);
uint64_t       core_now(core *);
core_counters *core_get_counters(core *);
void           core_clear_counters(core *);

#endif /* DYNAMIC_CORE_H_INCLUDED */
