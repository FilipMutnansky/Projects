// htab_foreach.c
// Riesenie IJC-DU2, priklad b), 13.4.2014 
// Autor: Filip Mutnansky, FIT
// Prelozene: gcc 4.6.4 
// Popis: Vykona zadanu funkciu nad kazdym
// 		  prvkom zoznamu

#include <stdio.h> 
#include "htab.h"

void htab_foreach(Thtab_t *t, void (*function)(const char *key, unsigned int value))
{
	unsigned int i;
	Thtab_listitem *tmp;
	
	for (i = 0; i < t->htab_size; i++)			// priechod polozkami htabulky
	{
		for (tmp = t->content[i]; tmp != NULL; tmp = tmp->next)		// priechod lin. zoznamom
		{
			function(tmp->key, tmp->data);							// vykonanie funkcie
		}	
	}	
	
	return;
}
