// htab_clear.c
// Riesenie IJC-DU2, priklad b), 13.4.2014 
// Autor: Filip Mutnansky, FIT
// Prelozene: gcc 4.6.4 
// Popis: Uvolni vsetky linearne zoznamy
// 		  v tabulke.

#include <stdio.h>
#include <stdlib.h>
#include "htab.h"

void htab_clear(Thtab_t *t)
{
	unsigned int i;
	Thtab_listitem *tmp = NULL, *tmp2 = NULL;
	
	for (i = 0; i < t->htab_size; i++)			// priechod tabulkou
	{
		tmp = t->content[i];					// prvy prvok v zozname
		
		while (tmp != NULL)						// pokym nenarazi na koniec
		{
			tmp2 = tmp->next;					// ulozenie nasledujuceho prvku
			free(tmp);							// uvolnenie tmp
			tmp = tmp2;							// priradenie nasledujuceho prvku
		}
		
		t->content[i] = NULL;					// oznacenie zoznamu ako prazdneho
	}
		
	return;	
}
