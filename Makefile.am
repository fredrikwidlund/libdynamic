LIB_CFLAGS = -O3 -flto -std=c11

if HAVE_FAT_LTO_OBJECTS
LIB_CFLAGS += -ffat-lto-objects
endif

LIB_TEST_CFLAGS = -O0 -g -ftest-coverage -fprofile-arcs -std=c11 -I/opt/local/include
LDADD_TEST_FLAGS = -L. -ldynamic_test -lcmocka -L/opt/local/lib

AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS} -I m4

SOURCE_FILES = src/vector.c src/string.c src/buffer.c src/map_int.c src/map_str.c src/cfarmhash.c src/io.c
HEADER_FILES = src/vector.h src/string.h src/buffer.h src/map_int.h src/map_str.h src/cfarmhash.h src/io.h

lib_LTLIBRARIES= libdynamic.la
libdynamic_la_CFLAGS = $(LIB_CFLAGS)
libdynamic_la_LDFLAGS = -static
libdynamic_la_SOURCES = $(SOURCE_FILES) $(HEADER_FILES)

check_LIBRARIES = libdynamic_test.a
libdynamic_test_a_CFLAGS = $(LIB_TEST_CFLAGS)
libdynamic_test_a_SOURCES = $(SOURCE_FILES) $(HEADER_FILES)

BUILD_TESTS = test/test_buffer test/test_vector test/test_string test/test_map_int test/test_map_str

TESTS = $(BUILD_TESTS) test/test_valgrind.sh test/test_coverage.sh

check_PROGRAMS = $(BUILD_TESTS)
check_SCRIPTS = test

test_test_buffer_CFLAGS = $(LIB_TEST_CFLAGS)
test_test_buffer_LDADD = $(LDADD_TEST_FLAGS)
test_test_buffer_LDFLAGS = -Wl,--wrap=malloc -Wl,--wrap=calloc -Wl,--wrap=realloc
test_test_buffer_SOURCES = test/test_buffer.c test/test_util.c

test_test_vector_CFLAGS = $(LIB_TEST_CFLAGS)
test_test_vector_LDADD = $(LDADD_TEST_FLAGS)
test_test_vector_LDFLAGS = -Wl,--wrap=malloc -Wl,--wrap=calloc -Wl,--wrap=realloc
test_test_vector_SOURCES = test/test_vector.c test/test_util.c

test_test_string_CFLAGS = $(LIB_TEST_CFLAGS)
test_test_string_LDADD = $(LDADD_TEST_FLAGS)
test_test_string_LDFLAGS = -Wl,--wrap=malloc -Wl,--wrap=calloc -Wl,--wrap=realloc
test_test_string_SOURCES = test/test_string.c test/test_util.c

test_test_map_int_CFLAGS =  $(LIB_TEST_CFLAGS)
test_test_map_int_LDADD = $(LDADD_TEST_FLAGS)
test_test_map_int_LDFLAGS = -Wl,--wrap=malloc -Wl,--wrap=calloc -Wl,--wrap=realloc
test_test_map_int_SOURCES = test/test_map_int.c test/test_util.c

test_test_map_str_CFLAGS =  $(LIB_TEST_CFLAGS)
test_test_map_str_LDADD = $(LDADD_TEST_FLAGS)
test_test_map_str_LDFLAGS = -Wl,--wrap=malloc -Wl,--wrap=calloc -Wl,--wrap=realloc
test_test_map_str_SOURCES = test/test_map_str.c test/test_util.c

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
