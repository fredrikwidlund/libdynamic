bin_PROGRAMS = map

map_SOURCES = map.c map_custom.c map_subclass.c map_dynamic.c map_unordered.cpp
map_CFLAGS = -std=gnu11 -I../src -g -O3 -flto -fuse-linker-plugin
map_CXXFLAGS = -I../src -g -O3 -flto -fuse-linker-plugin
map_LDADD = ../libdynamic.la

benchmarks: map.pdf

map.pdf: map.dat map.R
	./map.R

map.dat: map
	./$^ > $@

CLEANFILES = *.dat *.pdf
