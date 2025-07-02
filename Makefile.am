AUTOMAKE_OPTIONS		= subdir-objects serial-tests
ACLOCAL_AMFLAGS			= ${ACLOCAL_FLAGS} -I m4
DIST_SUBDIRS                    = example

HEADER_FILES 			= dynamic.h
mainheaderdir 			= $(includedir)
mainheader_HEADERS 		= $(HEADER_FILES)

pkgconfigdir 			= $(libdir)/pkgconfig
pkgconfig_DATA 			= libdynamic.pc

MAINTAINERCLEANFILES 		= aclocal.m4 config.h.in configure Makefile.in libdynamic-?.?.?.tar.gz
CLEANFILES 			= {.,test/,src/}/*.{gcno,gcda,gcov}

maintainer-clean-local:
	rm -rf autotools m4 libdynamic-?.?.?

### unit testing ###

AM_CFLAGS 			= -std=gnu23 -Wall -Werror -pedantic -g -flto -O0 -ftest-coverage -fprofile-arcs -I$(srcdir)/src -DGCOV_BUILD -DUNIT_TESTING
LDADD 				= -lcmocka

check_PROGRAMS 			= test/tests
dist_noinst_SCRIPTS             = test/valgrind.sh test/coverage.sh
TESTS 				= $(check_PROGRAMS) test/coverage.sh test/valgrind.sh
