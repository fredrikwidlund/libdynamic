#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <setjmp.h>

#include <cmocka.h>

int debug_out_of_memory = 0;
int debug_abort = 0;
int debug_recv = 0;
int debug_send = 0;
int debug_pthread_create = 0;
int debug_socketpair = 0;
int debug_epoll_create1 = 0;
int debug_epoll_ctl = 0;
int debug_epoll_wait = 0;

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

ssize_t __real_recv(int, void *, size_t, int);
ssize_t __wrap_recv(int socket, void *buffer, size_t length, int flags)
{
  return debug_recv ? -1 : __real_recv(socket, buffer, length, flags);
}

ssize_t __real_send(int, const void *, size_t, int);
ssize_t __wrap_send(int socket, const void *buffer, size_t length, int flags)
{
  return debug_send ? -1 : __real_send(socket, buffer, length, flags);
}

int __real_pthread_create(pthread_t *, const pthread_attr_t *, void *(*) (void *), void *);
int __wrap_pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg)
{
  return debug_pthread_create ? -1 : __real_pthread_create(thread, attr, start_routine, arg);
}

int __real_socketpair(int, int, int, int [2]);
int __wrap_socketpair(int domain, int type, int protocol, int socket_vector[2])
{
  return debug_socketpair ? -1 : __real_socketpair(domain, type, protocol, socket_vector);
}

int __real_epoll_create1(int);
int __wrap_epoll_create1(int flags)
{
  return debug_epoll_create1 ? -1 : __real_epoll_create1(flags);
}

int __real_epoll_ctl(int, int ,int, struct epoll_event *);
int __wrap_epoll_ctl(int epfd, int op, int fd, struct epoll_event *event)
{
  return debug_epoll_ctl ? -1 : __real_epoll_ctl(epfd, op, fd, event);
}

int __real_epoll_wait(int, struct epoll_event *, int, int);
int __wrap_epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout)
{
  return debug_epoll_wait ? -1 : __real_epoll_wait(epfd, events, maxevents, timeout);
}
