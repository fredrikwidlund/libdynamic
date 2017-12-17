ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS} -I m4
COMMON_FLAGS = -g -O3 -flto -fuse-linker-plugin -Isupport -I../src
AM_CFLAGS = -std=gnu11
AM_CXXFLAGS = -std=gnu++11
AM_CPPFLAGS = $(COMMON_FLAGS)
AM_LDFLAGS = $(COMMON_FLAGS)

bin_PROGRAMS = string_map list

string_map_SOURCES = \
map_str_ptr.c \
string_map.c

string_map_LDADD = ../libdynamic.la

list_SOURCES = \
list.c

list_LDADD = ../libdynamic.la

MAINTAINERCLEANFILES = Makefile.in
