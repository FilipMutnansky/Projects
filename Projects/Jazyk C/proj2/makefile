# Makefile
# Riesenie IJC-DU1, 13.4.2014
# Autor: Filip Mutnansky, FIT
# Popis: Makefile pre DU2

CC=gcc
CFLAGS=-std=c99 -pedantic -Wall -Wextra
CPP=g++
CPPFLAGS =-pedantic -Wall

all: fold fold2 wordcount wordcount-dynamic

fold: fold.o
	$(CC) $(CFLAGS) fold.o -o fold

fold.o: fold.c
	$(CC) $(CFLAGS) -c fold.c -o fold.o

hfunc.o: hfunc.c htab.h
	$(CC) $(CFLAGS) -c hfunc.c -o hfunc.o

htab_clear.o: htab_clear.c htab.h
	$(CC) $(CFLAGS) -c htab_clear.c -o htab_clear.o

htab_foreach.o: htab_foreach.c htab.h
	$(CC) $(CFLAGS) -c htab_foreach.c -o htab_foreach.o

htab_free.o: htab_free.c htab.h
	$(CC) $(CFLAGS) -c htab_free.c -o htab_free.o

htab_init.o: htab_init.c htab.h
	$(CC) $(CFLAGS) -c htab_init.c -o htab_init.o

htab_lookup.o: htab_lookup.c htab.h
	$(CC) $(CFLAGS) -c htab_lookup.c -o htab_lookup.o

htab_remove.o: htab_remove.c htab.h
	$(CC) $(CFLAGS) -c htab_remove.c -o htab_remove.o

htab_statistics.o: htab_statistics.c htab.h
	$(CC) $(CFLAGS) -c htab_statistics.c -o htab_statistics.o

libhtable.a: hfunc.o htab_clear.o htab_foreach.o htab_free.o htab_init.o htab_lookup.o htab_remove.o htab_statistics.o htab.h
	ar crs libhtable.a hfunc.o htab_clear.o htab_foreach.o htab_free.o htab_init.o htab_lookup.o htab_remove.o htab_statistics.o

libhtable.so: hfuncDyn.o htab_clearDyn.o htab_foreachDyn.o htab_freeDyn.o htab_initDyn.o htab_lookupDyn.o htab_removeDyn.o htab_statisticsDyn.o htab.h
	$(CC) -shared -fPIC hfuncDyn.o htab_clearDyn.o htab_foreachDyn.o htab_freeDyn.o htab_initDyn.o htab_lookupDyn.o htab_removeDyn.o htab_statisticsDyn.o -o libhtable.so

wordcount-dynamic: wordcount.o io.o libhtable.so
	$(CC) $(CFLAGS) -o wordcount-dynamic -dynamic wordcount.o io.o -L. -lhtable

hfuncDyn.o: hfunc.c htab.h
	$(CC) $(CFLAGS) -c -fPIC hfunc.c -o hfuncDyn.o

htab_clearDyn.o: htab_clear.c htab.h
	$(CC) $(CFLAGS) -c -fPIC htab_clear.c -o htab_clearDyn.o

htab_foreachDyn.o: htab_foreach.c htab.h
	$(CC) $(CFLAGS) -c -fPIC htab_foreach.c -o htab_foreachDyn.o

htab_freeDyn.o: htab_free.c htab.h
	$(CC) $(CFLAGS) -c -fPIC htab_free.c -o htab_freeDyn.o

htab_initDyn.o: htab_init.c htab.h
	$(CC) $(CFLAGS) -c -fPIC htab_init.c -o htab_initDyn.o

htab_lookupDyn.o: htab_lookup.c htab.h
	$(CC) $(CFLAGS) -c -fPIC htab_lookup.c -o htab_lookupDyn.o

htab_removeDyn.o: htab_remove.c htab.h
	$(CC) $(CFLAGS) -c -fPIC htab_remove.c -o htab_removeDyn.o

htab_statisticsDyn.o: htab_statistics.c htab.h
	$(CC) $(CFLAGS) -c -fPIC htab_statistics.c -o htab_statisticsDyn.o

wordcount.o: wordcount.c htab.h
	$(CC) $(CFLAGS) -c wordcount.c -o wordcount.o

wordcount: wordcount.o io.o libhtable.a
	$(CC) $(CFLAGS) -o wordcount -static wordcount.o io.o -L. -lhtable

io.o: io.c
	$(CC) $(CFLAGS) -c io.c -o io.o

io: io.c
	$(CC) $(CFLAGS) io.c -o io

fold2.o: fold2.cc
	$(CPP) $(CXXFLAGS) -c fold2.cc

fold2:	fold2.o
	$(CPP) $(CXXFLAGS) fold2.o -o fold2
