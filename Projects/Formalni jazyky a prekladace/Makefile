# prekladac.c
# Implementacia interpretu imperativneho jazyka IFJ14, 15.10.2014
# Autor: Viktor Toth,xtothv01, FIT
#        Peter Nagy,xnagyp01, FIT
#        Dusan Zeliar,xzelia00, FIT
#        Filip Mutnansky,xmutna00 , FIT
#        Marek Ochodek,xochod01, FIT
# Prelozene: gcc 4.8.2
# Makefile subor

# prekladac jazyka C
CC=gcc

#Parametre prekladaca
CFLAGS=-std=c99 -Wall -Wextra -pedantic -lm

all: prekladac clean
.PHONY: clean


#Zavislosti prekladac
prekladac: prekladac.o parsing_functions.o parsing_precedence.o parsing_topdown.o scanner.o error.o ial.o sem.o interpret.o functions.o
	$(CC) prekladac.o parsing_functions.o parsing_precedence.o parsing_topdown.o scanner.o error.o ial.o sem.o functions.o interpret.o -lm -o $@
prekladac.o: prekladac.c parsing_functions.c parsing_precedence.c parsing_topdown.c scanner.c ial.c sem.c interpret.c parsing.h scanner.h error.h ial.h sem.h interpret.h functions.h functions.c
	$(CC) $(CFLAGS) -c prekladac.c -o $@

#Zavislosti parsing_functions
parsing_functions.o: parsing_functions.c parsing.h scanner.h error.h
	$(CC) $(CFLAGS) -c parsing_functions.c -o $@

#Zavislosti parsing_precedence
parsing_precedence.o: parsing_precedence.c parsing.h scanner.h error.h sem.h ial.h interpret.h
	$(CC) $(CFLAGS) -c parsing_precedence.c -o $@

#Zavislosti parsing_topdown
parsing_topdown.o: parsing_topdown.c parsing.h scanner.h error.h sem.h ial.h
	$(CC) $(CFLAGS) -c parsing_topdown.c -o $@

#Zavislosti scanner
scanner.o: scanner.c parsing.h scanner.h error.h
	$(CC) $(CFLAGS) -c scanner.c -o $@
	
#Zavislosti ial	
ial.o: ial.c ial.h sem.h
	$(CC) $(CFLAGS) -c ial.c -o $@
	
#Zavislosti sem
sem.o: sem.c sem.h ial.h scanner.h
	$(CC) $(CFLAGS) -c sem.c -o $@	
	
#Zavislosti error
error.o: error.c error.h
	$(CC) $(CFLAGS) -c error.c -o $@

#Zavislosti interpret	
interpret.o: interpret.c interpret.h parsing.h error.h ial.h sem.h functions.h
	$(CC) $(CFLAGS) -c interpret.c -o $@
	
#Zavislosti functions
functions.o: functions.c functions.h error.h
	$(CC) $(CFLAGS) -c functions.c -o $@

clean:
	rm -f *.o 
