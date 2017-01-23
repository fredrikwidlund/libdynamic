ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS} -I m4
COMMON_FLAGS = -g -O3 -flto -fuse-linker-plugin -Isupport -I../src
AM_CFLAGS = -std=gnu11
AM_CXXFLAGS = -std=gnu++11
AM_CPPFLAGS = $(COMMON_FLAGS)
AM_LDFLAGS = $(COMMON_FLAGS)

bin_PROGRAMS = vector map

vector_SOURCES = \
vector.c vector_dynamic.c vector_stdlib.cpp \
vector.h vector_dynamic.h vector_stdlib.h \
vector.R
vector_LDADD = ../libdynamic.la

map_SOURCES = \
map.c map_std_unordered.cpp map_custom.c map_libdynamic.c map_libdynamic_subclass.c \
map.h map_std_unordered.h map_custom.h map_libdynamic.h  map_libdynamic_subclass.h \
map.R
map_LDADD = ../libdynamic.la

benchmark: vector.pdf map.pdf

vector.pdf: vector.dat vector.R
	./vector.R

vector.dat: vector
	./$^ > $@

map.pdf: map.dat map.R
	./map.R

map.dat: map
	./$^ > $@

CLEANFILES = *.dat *.pdf
