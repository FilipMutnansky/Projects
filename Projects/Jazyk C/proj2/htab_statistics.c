#include <stdio.h>
#include <stdbool.h>
// htab_init.c
// Riesenie IJC-DU2, priklad b), 13.4.2014 
// Autor: Filip Mutnansky, FIT
// Prelozene: gcc 4.6.4 
// Popis: Vypise maximalnu/minimalnu/priemernu 
//        dlzku zoznamu v tabulke 

#include <limits.h>
#include "htab.h"

void htab_statistics(Thtab_t *t)
{
	unsigned int i, itemsOnList = 0,itemsOnTable = 0, max = 0, min = UINT_MAX;
	Thtab_listitem *tmp = NULL;
	
	for (i = 0; i < t->htab_size; i++)							// priechod polozkami htabulky
	{
		for (tmp = t->content[i]; tmp != NULL; tmp = tmp->next)	// priechod lin. zoznamom
		{
			itemsOnList++;
			itemsOnTable++;
		}	
		
		if (itemsOnList > max)			// MAX
			max = itemsOnList;
			
		if (itemsOnList < min)			// MIN
			min = itemsOnList;
			
		itemsOnList = 0;
	}	
	
	printf("min:\t\t%u\n",min);												// vypis min:\t\thodnota
	printf("max:\t\t%u\n",max);												// vypis max:\t\thodnota
	printf("priemer:\t%g\n",(double)itemsOnTable/(double)t->htab_size);		// vypis priemer:\thodnota
	
	return;
}
