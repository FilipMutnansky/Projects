// htab_remove.c
// Riesenie IJC-DU2, priklad b), 13.4.2014 
// Autor: Filip Mutnansky, FIT
// Prelozene: gcc 4.6.4 
// Popis: Zmaze polozku podla zadaneho mena

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "htab.h"

void htab_remove(Thtab_t *t,const char *key)
{
	unsigned int index;
	Thtab_listitem *tmp = NULL, *tmp2 = NULL;
	bool found;
	
	index = hfunc(key, t->htab_size);					// zistenie indexu v hash. tabulke pomocou kluca
	
	for (tmp = t->content[index]; tmp != NULL; tmp = tmp->next)			// priechod lin. zoznamom
	{
		if (!strcmp(key,tmp->key))										// ak najde pozadovanu polozku
		{
			found = true;
			break;
		}
		
		tmp2 = tmp;					  // predchadzajuca polozka
	}
	
	if (found)
	{
		if (tmp2 == NULL)             // ak ide o prvu polozku
		{
			t->content[index] = tmp->next;		// zaciatok bude obsahovat nasledujucu polozku	
			free(tmp);							// uvolnenie tmp
		}
		else  									// ak ide o polozku uprostred zoznamu
			{
				tmp2->next = tmp->next;			// "vynechanie" polozky tmp zo zoznamu
				free(tmp);						// uvolnenie tmp
			}
	}
	
	return;	
}
