LIB_CFLAGS = -O3 -flto -std=c11 -I$(top_srcdir)/src

if HAVE_FAT_LTO_OBJECTS
LIB_CFLAGS += -ffat-lto-objects
endif

LIB_TEST_CFLAGS = -O0 -g -ftest-coverage -fprofile-arcs -std=c11 -I$(top_srcdir)/src -I/opt/local/include
LDADD_TEST_FLAGS = -L. -ldynamic_test -lcmockery -L/opt/local/lib

AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS} -I m4

SOURCE_FILES = src/vector.c src/buffer.c
HEADER_FILES = src/vector.h src/buffer.h

lib_LTLIBRARIES= libdynamic.la
libdynamic_la_CFLAGS = $(LIB_CFLAGS)
libdynamic_la_LDFLAGS = -static
libdynamic_la_SOURCES = $(SOURCE_FILES) $(HEADER_FILES)

check_LIBRARIES = libdynamic_test.a
libdynamic_test_a_CFLAGS = $(LIB_TEST_CFLAGS)
libdynamic_test_a_SOURCES = $(SOURCE_FILES) $(HEADER_FILES)

BUILD_TESTS = test/test_buffer test/test_vector

TESTS = $(BUILD_TESTS) test/test_valgrind.sh test/test_coverage.sh

check_PROGRAMS = $(BUILD_TESTS)
check_SCRIPTS = test

test_test_buffer_CFLAGS = $(LIB_TEST_CFLAGS)
test_test_buffer_LDADD = $(LDADD_TEST_FLAGS)

test_test_vector_CFLAGS = $(LIB_TEST_CFLAGS)
test_test_vector_LDADD = $(LDADD_TEST_FLAGS)

headerfilesdir = $(includedir)/dynamic
headerfiles_HEADERS = $(HEADER_FILES)

mainheaderdir = $(includedir)
mainheader_HEADERS = src/dynamic.h

dist_man3_MANS = man/vector.3

dist_noinst_SCRIPTS = autogen.sh test/test_valgrind.sh test/test_coverage.sh

CLEANFILES = {./,src,test}/*.{gcno,gcda,gcov}
MAINTAINERCLEANFILES = aclocal.m4 config.h.in configure Makefile.in libdynamic-*.tar.gz INSTALL

maintainer-clean-local:
	rm -rf autotools m4 libdynamic-0.1.0
