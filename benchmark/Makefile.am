bin_PROGRAMS = map_dynamic map_unordered

map_unordered_SOURCES = map_unordered.cpp
map_unordered_CXXFLAGS = -I../src -g -march=native -mtune=native -O3 -flto -fuse-linker-plugin

map_dynamic_SOURCES = map_dynamic.c
map_dynamic_CFLAGS = -std=gnu11 -I../src -g -march=native -mtune=native -O3 -flto -fuse-linker-plugin
map_dynamic_LDADD = ../libdynamic.la

benchmarks: map.pdf

map.pdf: map_unordered.dat map_dynamic.dat
	./map.R

map_unordered.dat: map_unordered
	./map_unordered > $@

map_dynamic.dat: map_dynamic
	./map_dynamic > $@

CLEANFILES = *.dat *.pdf
