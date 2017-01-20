bin_PROGRAMS = vector map

vector_SOURCES = \
vector.c vector_dynamic.c vector_stdlib.cpp \
vector.h vector_dynamic.h vector_stdlib.h \
vector.R
vector_CFLAGS = -std=gnu11 -I../src -g -O3 -flto -fuse-linker-plugin
vector_CXXFLAGS = -I../src -g -O3 -flto -fuse-linker-plugin
vector_LDADD = ../libdynamic.la

map_SOURCES = \
map.c map_std_unordered.cpp map_custom.c map_dynamic.c \
map.h map_std_unordered.h map_custom.h map_dynamic.h \
map.R
#map.c map_custom.c map_subclass.c map_dynamic.c map_unordered.cpp \
#map.h map_custom.h map_subclass.h map_dynamic.h map_unordered.h \
#map.R
map_CFLAGS = -std=gnu11 -I../src -g -O3 -flto -fuse-linker-plugin
map_CXXFLAGS = -I../src -g -O3 -flto -fuse-linker-plugin
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
