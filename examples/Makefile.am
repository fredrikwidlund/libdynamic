ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS} -I m4
COMMON_FLAGS = -g -O3 -flto -Isupport -I../src
AM_CFLAGS = -std=gnu11
AM_CXXFLAGS = -std=gnu++11
AM_CPPFLAGS = $(COMMON_FLAGS)
AM_LDFLAGS = $(COMMON_FLAGS)

bin_PROGRAMS = maps mapi list async

maps_SOURCES = maps.c
maps_LDADD = ../libdynamic.la

mapi_SOURCES = mapi.c
mapi_LDADD = ../libdynamic.la

list_SOURCES = list.c
list_LDADD = ../libdynamic.la

async_SOURCES = async.c
async_LDADD = ../libdynamic.la
async_LDFLAGS = -pthread

MAINTAINERCLEANFILES = Makefile.in
