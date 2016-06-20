// prvocisla.c
// Riesenie IJC-DU1, priklad a), 19.3.2014 
// Autor: Filip Mutnansky, FIT
// Přeloženo: gcc 4.6.4
// Popis: Vypocet prvocisel pomocou bitovych poli

#include <stdio.h>
#include "bit-array.h"
#include "eratosthenes.c"

#define N 101000000
#define NUMBER_COUNT 10

int main(void)
{		
	BitArray(p,N);
	int counter = 0,j,i;
	unsigned long long pole[NUMBER_COUNT];
					
	Eratosthenes(p);   // nastavenie bitov podla prvocisel
		
	i = N;
	
	// ulozenie do pola, kvoli vypisaniu vzostupne
	
	while (counter < NUMBER_COUNT)
	{
		if (GetBit(p,i) == (unsigned long)0)
		{
			pole[counter]=i;
			counter++;
		}		
		i--;
	}
	
	j = NUMBER_COUNT - 1;
	
	while (j >= 0)
	{
		printf("%llu\n",pole[j]);  // vypis prvocisel
		j--;
	}

	return 0;
}
