ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS} -I m4
AM_CFLAGS = -std=gnu11 -g -O3 -flto -fuse-linker-plugin
AM_LDFLAGS = -static

DIST_SUBDIRS = docs benchmark

EXTRA_DIST = \
CHANGES \
LICENSE \
README.rst

SOURCE_FILES = \
src/dynamic/vector.c \
src/dynamic/string.c \
src/dynamic/buffer.c \
src/dynamic/map.c \
src/dynamic/hash.c

HEADER_FILES = \
src/dynamic/vector.h \
src/dynamic/string.h \
src/dynamic/buffer.h \
src/dynamic/map.h \
src/dynamic/hash.h

AUTOMAKE_OPTIONS = subdir-objects
lib_LTLIBRARIES= libdynamic.la
libdynamic_la_SOURCES = $(SOURCE_FILES) $(HEADER_FILES)

headerfilesdir = $(includedir)/dynamic
headerfiles_HEADERS = $(HEADER_FILES)

mainheaderdir = $(includedir)
mainheader_HEADERS = src/dynamic.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libdynamic.pc

MAINTAINERCLEANFILES = aclocal.m4 config.h.in configure Makefile.in docs/Makefile.in benchmark/Makefile.in libdynamic-?.?.?.tar.gz
maintainer-clean-local:; rm -rf autotools m4 libdynamic-?.?.?

### unit tests ###

CHECK_CFLAGS = -std=gnu11 -O0 -g -ftest-coverage -fprofile-arcs
CHECK_LDADD = -L. -ldynamic_test -lcmocka
CHECK_LDFLAGS_EXTRA = -Wl,--wrap=malloc -Wl,--wrap=calloc -Wl,--wrap=realloc -Wl,--wrap=aligned_alloc -Wl,--wrap=abort

check_LIBRARIES = libdynamic_test.a
libdynamic_test_a_CFLAGS = $(CHECK_CFLAGS)
libdynamic_test_a_SOURCES = $(SOURCE_FILES) $(HEADER_FILES)

check_PROGRAMS = test/buffer
test_buffer_CFLAGS = $(CHECK_CFLAGS)
test_buffer_LDADD = $(CHECK_LDADD)
test_buffer_LDFLAGS = $(CHECK_LDFLAGS_EXTRA)
test_buffer_SOURCES = test/buffer.c test/mock.c

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

check_PROGRAMS += test/hash
test_hash_CFLAGS = $(CHECK_CFLAGS)
test_hash_LDADD = $(CHECK_LDADD)
test_hash_LDFLAGS = $(CHECK_LDFLAGS_EXTRA)
test_hash_SOURCES = test/hash.c test/mock.c

check_PROGRAMS += test/map
test_map_CFLAGS = $(CHECK_CFLAGS)
test_map_LDADD = $(CHECK_LDADD)
test_map_LDFLAGS = $(CHECK_LDFLAGS_EXTRA)
test_map_SOURCES = test/map.c test/mock.c

dist_noinst_SCRIPTS = test/valgrind.sh test/coverage.sh

TESTS = $(check_PROGRAMS) test/coverage.sh test/valgrind.sh

CLEANFILES = {.,src/dynamic,test}/*.{gcno,gcda,gcov}
