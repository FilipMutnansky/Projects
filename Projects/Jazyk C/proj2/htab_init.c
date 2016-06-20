// htab_init.c
// Riesenie IJC-DU2, priklad b), 13.4.2014 
// Autor: Filip Mutnansky, FIT
// Prelozene: gcc 4.6.4 
// Popis: Inicializuje hashovaciu tabulku

#include <stdio.h>
#include <stdlib.h>
#include "htab.h"

Thtab_t *htab_init(unsigned int size)
{
	Thtab_t *t = NULL;
	unsigned int i;
		
	if ((t = malloc(sizeof(Thtab_t) + size*sizeof(Thtab_listitem *))) == NULL)		// alokovanie potrebneho miesta na tabulku + miesta na pointery
		return NULL;
		
	for (i = 0; i < size; i++)
	{
		t->content[i] = NULL;						// oznacenie polozky v tabulke ako prazdnej
	}
	
	t->htab_size = size;							// priradenie velkosti do struktury
		
	return t;
}
