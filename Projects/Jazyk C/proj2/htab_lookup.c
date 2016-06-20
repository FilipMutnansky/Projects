// htab_lookup.c
// Riesenie IJC-DU2, priklad b), 13.4.2014 
// Autor: Filip Mutnansky, FIT
// Prelozene: gcc 4.6.4 
// Popis: Vyhlada pozadovane slovo v tabulke
// 		  ak ho najde zvisi pocet vyskytov, ak
// 		  nie vytvori novu polozku.

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "htab.h"

Thtab_listitem *htab_lookup(Thtab_t *t, const char *key)
{
	unsigned int index;
	Thtab_listitem *tmp = NULL, *tmp2 = NULL;
	bool found;
	
	index = hfunc(key, t->htab_size);			// zistenie indexu do hashovacej tabulky
	
	if (t->content[index] == NULL)				// ak je polozka v hash. tabulke prazdna -> vytvori novu
	{
		Thtab_listitem *newListItem = NULL;
		
		if ((newListItem = malloc((sizeof(Thtab_listitem) + strlen(key) + 1))) == NULL)   // alokovanie potrebneho miesta (+1 kvoli nullbyte)
			return NULL;
		
		strncpy(newListItem->key, key, strlen(key));			  // kopia key do polozky
		newListItem->data = 1;									  // pocet vyskytov nastaveny na 1
		newListItem->next = NULL;								  // nasledujuci prvok v zozname je NULL (neexistuje)
		
		t->content[index] = newListItem;						  // zaradenie do struktury
		   		
		return newListItem;
	}
																	// ak polozka v hash. tabulke je uz vytvorena
	for (tmp = t->content[index]; tmp != NULL; tmp = tmp->next)		// priechod lin. zoznamom
	{
		if (!strcmp(key,tmp->key))									// ak sa nasiel pozadovany zaznam
		{
			found = true;
			break;
		}
		
		tmp2 = tmp;													// predch. polozka
	}
	
	if (found)
	{
		tmp->data++;												// zvysenie vyskytu
		return tmp;
	}
		else                                                        // neexistuje este polozka v zozname
			{
				Thtab_listitem *newListItem;
		
				if ((newListItem = malloc((sizeof(Thtab_listitem) + strlen(key) + 1))) == NULL) // alokovanie potrebneho miesta (+1 kvoli nullbyte)
					return NULL;
		
				strncpy(newListItem->key, key, strlen(key));	// kopia key do polozky
				newListItem->data = 1;							// pocet vyskytov nastaveny na 1
				newListItem->next = NULL;						// nasledujuci prvok v zozname je NULL (neexistuje)
				
				tmp2->next = newListItem;						// zaradenie do struktury
				return newListItem;
			}	
}
