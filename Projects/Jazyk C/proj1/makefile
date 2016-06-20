# Makefile
# Riesenie IJC-DU1, 19.3.2014
# Autor: Filip Mutnansky, FIT
# Popis: Makefile pre DU1

CC=gcc
CFLAGS=-std=c99 -pedantic -W -Wall -Wextra -O2 -lm

all: prvocisla prvocisla-inline steg-decode

prvocisla: prvocisla.o eratosthenes.o error.o
	$(CC) $(CFLAGS) prvocisla.o eratosthenes.o error.o -lm -o $@

prvocisla.o: prvocisla.c bit-array.h
	$(CC) $(CFLAGS) -c prvocisla.c -o $@

prvocisla-inline: prvocisla-inline.o eratosthenes.o error.o
	$(CC) $(CFLAGS) prvocisla-inline.o eratosthenes.o error.o -lm -o $@

prvocisla-inline.o: prvocisla.c eratosthenes.c bit-array.h
	$(CC) $(CFLAGS) -DUSE_INLINE -c prvocisla.c -o $@

error.o: error.c error.h
	$(CC) $(CFLAGS) -c error.c -o $@

steg-decode: steg-decode.o eratosthenes.o error.o ppm.o
	$(CC) $(CFLAGS) steg-decode.o eratosthenes.o error.o ppm.o -lm -o $@

steg-decode.o: steg-decode.c ppm.h bit-array.h
	$(CC) $(CFLAGS) -c steg-decode.c -o $@

ppm.o: ppm.c ppm.h error.h
	$(CC) $(CFLAGS) -c ppm.c -o $@

.PHONY: clean clean-all

clean:
	rm -f *.o

clean-all:
	rm -f *.o prvocisla prvocisla-inline steg-decode
