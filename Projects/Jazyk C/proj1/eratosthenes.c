// eratosthenes.c
// Riesenie IJC-DU1, priklad a), 19.3.2014 
// Autor: Filip Mutnansky, FIT
// Prelozene: gcc 4.6.4 
// Popis: Nastavi bity z neprvociselnym indexom
// na 1

#include <stdio.h>
#include <math.h>
#include "bit-array.h"

static void Eratosthenes(BitArray_t pole)
{
	unsigned long long i,n;
	
	for (i = 2; i<= sqrt(BitArraySize(pole)) ; i++)   // od 2 do sqrt(N)
		{
			if (GetBit(pole,i) == 0)
				for (n = 2; n*i <= (BitArraySize(pole)); n++) 
				{
					SetBit(pole, i*n,1);  // "vyskrta nasobky prvocisla"
				}
		}	
	return;
}
