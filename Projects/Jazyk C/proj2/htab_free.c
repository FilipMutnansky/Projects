// htab_free.c
// Riesenie IJC-DU2, priklad b), 13.4.2014 
// Autor: Filip Mutnansky, FIT
// Prelozene: gcc 4.6.4 
// Popis: Uvolni celu hashovaciu tabulku
//        (vola htab_clear)

#include <stdio.h>
#include <stdlib.h>
#include "htab.h"

void htab_free(Thtab_t *t)
{
	htab_clear(t);			// uvolni jednotlive zoznamy polozku po polozke
	free(t);				// uvolni celu tabulku
	return;
}
