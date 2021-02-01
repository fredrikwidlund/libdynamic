#ifndef DYNAMIC_H_INCLUDED
#define DYNAMIC_H_INCLUDED

#define DYNAMIC_VERSION "2.3.0"
#define DYNAMIC_VERSION_MAJOR 2
#define DYNAMIC_VERSION_MINOR 3
#define DYNAMIC_VERSION_PATCH 0

#define dynamic_likely(x)   __builtin_expect(!!(x), 1)
#define dynamic_unlikely(x) __builtin_expect(!!(x), 0)

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <unistd.h>
#include <pthread.h>

#include <dynamic/segment.h>
#include <dynamic/utility.h>
#include <dynamic/hash.h>
#include <dynamic/buffer.h>
#include <dynamic/list.h>
#include <dynamic/vector.h>
#include <dynamic/string.h>
#include <dynamic/map.h>
#include <dynamic/maps.h>
#include <dynamic/mapi.h>
#include <dynamic/core.h>
#include <dynamic/pool.h>

#ifdef __cplusplus
}
#endif

#endif /* DYNAMIC_H_INCLUDED */
