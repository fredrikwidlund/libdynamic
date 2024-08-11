AUTOMAKE_OPTIONS		= subdir-objects serial-tests
ACLOCAL_AMFLAGS			= ${ACLOCAL_FLAGS} -I m4
DIST_SUBDIRS                    = example

HEADER_FILES = \
src/dynamic/hash.h \
src/dynamic/data.h \
src/dynamic/buffer.h \
src/dynamic/vector.h \
src/dynamic/list.h \
src/dynamic/map.h

headerfilesdir                  = $(includedir)/dynamic
headerfiles_HEADERS             = $(HEADER_FILES)

mainheaderdir 			= $(includedir)
mainheader_HEADERS 		= src/dynamic.h

MAINTAINERCLEANFILES 		= aclocal.m4 config.h.in configure Makefile.in libdynamic-?.?.?.tar.gz

CLEANFILES 			= {.,test/,src/dynamic/}/*.{gcno,gcda,gcov}

maintainer-clean-local:
	rm -rf autotools m4 libdynamic-?.?.?

### unit testing ###

AM_CFLAGS 			= -std=gnu2x -Wall -Werror -pedantic -g -flto -O0 -ftest-coverage -fprofile-arcs -I$(srcdir)/src -DGCOV_BUILD -DUNIT_TESTING
LDADD 				= -lcmocka

check_PROGRAMS 			= \
test/hash \
test/data \
test/buffer \
test/vector \
test/list \
test/map

dist_noinst_SCRIPTS             = test/valgrind.sh test/coverage.sh

TESTS 				= $(check_PROGRAMS) test/coverage.sh test/valgrind.sh
