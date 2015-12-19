ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS} -I m4
AM_CFLAGS = -std=gnu11 -O3 -flto -fuse-linker-plugin
AM_LDFLAGS = -static

SOURCE_FILES = \
src/dynamic/vector.c \
src/dynamic/string.c \
src/dynamic/buffer.c \
src/dynamic/mapi.c \
src/dynamic/maps.c \
src/dynamic/cfarmhash.c

HEADER_FILES = \
src/dynamic/vector.h \
src/dynamic/string.h \
src/dynamic/buffer.h \
src/dynamic/mapi.h \
src/dynamic/maps.h \
src/dynamic/cfarmhash.h

AUTOMAKE_OPTIONS = subdir-objects
lib_LTLIBRARIES= libdynamic.la
libdynamic_la_SOURCES = $(SOURCE_FILES) $(HEADER_FILES)

headerfilesdir = $(includedir)/dynamic
headerfiles_HEADERS = $(HEADER_FILES)

mainheaderdir = $(includedir)
mainheader_HEADERS = src/dynamic.h

MAINTAINERCLEANFILES = aclocal.m4 config.h.in configure Makefile.in libdynamic-?.?.?.tar.gz
maintainer-clean-local:; rm -rf autotools m4 libdynamic-?.?.?

### unit tests ###

CHECK_CFLAGS = -std=gnu11 -O0 -g -ftest-coverage -fprofile-arcs
CHECK_LDADD = -L. -ldynamic_test -lcmocka
CHECK_LDFLAGS_EXTRA = -Wl,--wrap=malloc -Wl,--wrap=calloc -Wl,--wrap=realloc -Wl,--wrap=aligned_alloc

check_LIBRARIES = libdynamic_test.a
libdynamic_test_a_CFLAGS = $(CHECK_CFLAGS)
libdynamic_test_a_SOURCES = $(SOURCE_FILES) $(HEADER_FILES)

check_PROGRAMS = test/buffer
test_buffer_CFLAGS = $(CHECK_CFLAGS)
test_buffer_LDADD = $(CHECK_LDADD)
test_buffer_LDFLAGS = $(CHECK_LDFLAGS_EXTRA)
test_buffer_SOURCES = test/buffer.c test/memory.c

check_PROGRAMS += test/vector
test_vector_CFLAGS = $(CHECK_CFLAGS)
test_vector_LDADD = $(CHECK_LDADD)
test_vector_LDFLAGS = $(CHECK_LDFLAGS_EXTRA)
test_vector_SOURCES = test/vector.c test/memory.c

check_PROGRAMS += test/string
test_string_CFLAGS = $(CHECK_CFLAGS)
test_string_LDADD = $(CHECK_LDADD)
test_string_LDFLAGS = $(CHECK_LDFLAGS_EXTRA)
test_string_SOURCES = test/string.c test/memory.c

check_PROGRAMS += test/mapi
test_mapi_CFLAGS = $(CHECK_CFLAGS)
test_mapi_LDADD = $(CHECK_LDADD)
test_mapi_LDFLAGS = $(CHECK_LDFLAGS_EXTRA)
test_mapi_SOURCES = test/mapi.c test/memory.c

check_PROGRAMS += test/maps
test_maps_CFLAGS = $(CHECK_CFLAGS)
test_maps_LDADD = $(CHECK_LDADD)
test_maps_LDFLAGS = $(CHECK_LDFLAGS_EXTRA)
test_maps_SOURCES = test/maps.c test/memory.c

dist_noinst_SCRIPTS = test/valgrind.sh test/coverage.sh

TESTS = $(check_PROGRAMS) test/coverage.sh test/valgrind.sh

CLEANFILES = {.,src/dynamic,test}/*.{gcno,gcda,gcov}
