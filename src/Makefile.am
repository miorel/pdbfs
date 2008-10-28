## Process this file with automake to produce Makefile.in
AM_CPPFLAGS = -I/usr/include/fuse -pthread
bin_PROGRAMS = pdbfs
pdbfs_SOURCES = impl.c main.c rep.c rsync.c updater.c util.c fs.h rep.h rsync.h updater.h util.h
pdbfs_LDADD = -lfuse -lpthread -lz

