AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS} -I m4
AM_CFLAGS = -std=gnu11 -g -O3 -march=native -flto -I$(srcdir)/src
AM_LDFLAGS = -static

DIST_SUBDIRS = docs examples

EXTRA_DIST = \
CHANGES \
LICENSE \
README.rst

SOURCE_FILES = \
src/dynamic/segment.c \
src/dynamic/utility.c \
src/dynamic/hash.c \
src/dynamic/buffer.c \
src/dynamic/list.c \
src/dynamic/vector.c \
src/dynamic/string.c \
src/dynamic/map.c \
src/dynamic/maps.c \
src/dynamic/mapi.c \
src/dynamic/pool.c \
src/dynamic/core.c

HEADER_FILES = \
src/dynamic/segment.h \
src/dynamic/utility.h \
src/dynamic/hash.h \
src/dynamic/buffer.h \
src/dynamic/list.h \
src/dynamic/vector.h \
src/dynamic/string.h \
src/dynamic/map.h \
src/dynamic/maps.h \
src/dynamic/mapi.h \
src/dynamic/pool.h \
src/dynamic/core.h

lib_LTLIBRARIES= libdynamic.la
libdynamic_la_SOURCES = $(SOURCE_FILES) $(HEADER_FILES)

headerfilesdir = $(includedir)/dynamic
headerfiles_HEADERS = $(HEADER_FILES)

mainheaderdir = $(includedir)
mainheader_HEADERS = src/dynamic.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libdynamic.pc

MAINTAINERCLEANFILES = aclocal.m4 config.h.in configure Makefile.in docs/Makefile.in benchmark/Makefile.in examples/Makefile.in libdynamic-?.?.?.tar.gz
maintainer-clean-local:; rm -rf autotools m4 libdynamic-?.?.?

### unit tests ###

check_LIBRARIES = libdynamic_test.a
libdynamic_test_a_CFLAGS = $(CHECK_CFLAGS)
libdynamic_test_a_SOURCES = $(SOURCE_FILES) $(HEADER_FILES)

CHECK_CFLAGS = -std=gnu11 -O0 -g -ftest-coverage -fprofile-arcs -I$(srcdir)/src -DGCOV_BUILD
CHECK_LDADD = libdynamic_test.a -lcmocka
CHECK_LDFLAGS_EXTRA = \
-Wl,--wrap=malloc \
-Wl,--wrap=calloc \
-Wl,--wrap=realloc \
-Wl,--wrap=aligned_alloc \
-Wl,--wrap=abort \
-Wl,--wrap=recv \
-Wl,--wrap=send \
-Wl,--wrap=pthread_create \
-Wl,--wrap=socketpair \
-Wl,--wrap=epoll_create1 \
-Wl,--wrap=epoll_ctl \
-Wl,--wrap=epoll_wait \
-pthread

check_PROGRAMS =

check_PROGRAMS += test/segment
test_segment_CFLAGS = $(CHECK_CFLAGS)
test_segment_LDADD = $(CHECK_LDADD)
test_segment_LDFLAGS = $(CHECK_LDFLAGS_EXTRA)
test_segment_SOURCES = test/segment.c test/mock.c

check_PROGRAMS += test/utility
test_utility_CFLAGS = $(CHECK_CFLAGS)
test_utility_LDADD = $(CHECK_LDADD)
test_utility_LDFLAGS = $(CHECK_LDFLAGS_EXTRA)
test_utility_SOURCES = test/utility.c test/mock.c

check_PROGRAMS += test/hash
test_hash_CFLAGS = $(CHECK_CFLAGS)
test_hash_LDADD = $(CHECK_LDADD)
test_hash_LDFLAGS = $(CHECK_LDFLAGS_EXTRA)
test_hash_SOURCES = test/hash.c test/mock.c

check_PROGRAMS += test/buffer
test_buffer_CFLAGS = $(CHECK_CFLAGS)
test_buffer_LDADD = $(CHECK_LDADD)
test_buffer_LDFLAGS = $(CHECK_LDFLAGS_EXTRA)
test_buffer_SOURCES = test/buffer.c test/mock.c

check_PROGRAMS += test/list
test_list_CFLAGS = $(CHECK_CFLAGS)
test_list_LDADD = $(CHECK_LDADD)
test_list_LDFLAGS = $(CHECK_LDFLAGS_EXTRA)
test_list_SOURCES = test/list.c test/mock.c

check_PROGRAMS += test/vector
test_vector_CFLAGS = $(CHECK_CFLAGS)
test_vector_LDADD = $(CHECK_LDADD)
test_vector_LDFLAGS = $(CHECK_LDFLAGS_EXTRA)
test_vector_SOURCES = test/vector.c test/mock.c

check_PROGRAMS += test/string
test_string_CFLAGS = $(CHECK_CFLAGS)
test_string_LDADD = $(CHECK_LDADD)
test_string_LDFLAGS = $(CHECK_LDFLAGS_EXTRA)
test_string_SOURCES = test/string.c test/mock.c

check_PROGRAMS += test/maps
test_maps_CFLAGS = $(CHECK_CFLAGS)
test_maps_LDADD = $(CHECK_LDADD)
test_maps_LDFLAGS = $(CHECK_LDFLAGS_EXTRA)
test_maps_SOURCES = test/maps.c test/mock.c

check_PROGRAMS += test/mapi
test_mapi_CFLAGS = $(CHECK_CFLAGS)
test_mapi_LDADD = $(CHECK_LDADD)
test_mapi_LDFLAGS = $(CHECK_LDFLAGS_EXTRA)
test_mapi_SOURCES = test/mapi.c test/mock.c

check_PROGRAMS += test/map
test_map_CFLAGS = $(CHECK_CFLAGS)
test_map_LDADD = $(CHECK_LDADD)
test_map_LDFLAGS = $(CHECK_LDFLAGS_EXTRA)
test_map_SOURCES = test/map.c test/mock.c

check_PROGRAMS += test/pool
test_pool_CFLAGS = $(CHECK_CFLAGS)
test_pool_LDADD = $(CHECK_LDADD)
test_pool_LDFLAGS = $(CHECK_LDFLAGS_EXTRA)
test_pool_SOURCES = test/pool.c test/mock.c

check_PROGRAMS += test/core
test_core_CFLAGS = $(CHECK_CFLAGS)
test_core_LDADD = $(CHECK_LDADD)
test_core_LDFLAGS = $(CHECK_LDFLAGS_EXTRA)
test_core_SOURCES = test/core.c test/mock.c

dist_noinst_SCRIPTS = test/valgrind.sh test/coverage.sh

TESTS = $(check_PROGRAMS) test/coverage.sh test/valgrind.sh

CLEANFILES = {.,src/dynamic,test}/*.{gcno,gcda,gcov}
