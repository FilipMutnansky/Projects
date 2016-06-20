// functions.c
 // Implementacia interpretu imperativneho jazyka IFJ14, 15.10.2014
 // Autor: Viktor Toth,xtothv01, FIT
 //        Peter Nagy,xnagyp01, FIT
 //        Dusan Zeliar,xzelia00, FIT
 //        Filip Mutnansky,xmutna00 , FIT
 //        Marek Ochodek,xochod01, FIT
 // Prelozene: gcc 4.8.2
 // .c subor functions.c
 // vestavene funkce:       find, sort, hash Table

#include "ial.h"
#include "sem.h"
#include "error.h"

void vypocetNajPola(char *search, int dlzkaSearch, int *pomocnePole);

/*
 * find - Vyhladá prvý vyskyt zadaného podretazca search v retazci string a vráti jeho pozicu (pocítane od jednicky)
 * @param char *string - zadany retezec
 * @param char *search - hladany podretazec
 * @return int - vrati první výskyt zadaného podretazca (počítané od jedničky)
 */
int find(char *string, char *search)
{
    int dlzkaSearch = strlen(search);
    int dlzkaString = strlen(string);

    // prazdny retazec
    if(dlzkaSearch == 0)
    {
        return 1;
    }
	//vytovríme pomocnePole[] ,ktory bude obsahovat najdlhsie hodnoty pre hladany vyraz
    int *pomocnePole = (int *)malloc(sizeof(int)*dlzkaSearch);		// pomocnePole == najdlhsi vyraz
    if(pomocnePole == NULL)
    {
        InternalError("Internal error - Nie je k dispozicii dostatok pamate");
        return 0;
    }

    int j  = 0;  												// index pre search[]

    // vypocitanie pomocnePole[] pola
    vypocetNajPola(search, dlzkaSearch, pomocnePole);

    int i = 0;  						// index pre string[]
    while(i < dlzkaString)
    {
      if(search[j] == string[i])
      {
        j++;
        i++;
      }

      if (j == dlzkaSearch)
      {
        free(pomocnePole);
        return (i-j+1);					//uspech
      }
      else if(search[j] != string[i])
      {
        if(j != 0)
         j = pomocnePole[j-1];
        else
         i++;
      }
    }
    free(pomocnePole);					    //neuspech
    return 0;

}
// Patrí k funkcii FIND
void vypocetNajPola(char *search, int dlzkaSearch, int *pomocnePole)
{
    int dlzka = 0;  					//dlzka predchadzajuceho najdlhsieho pola
    int i = 1;

    pomocnePole[0] = 0; 						// pomocnePole[0] je vzdy 0

	//v cykle vypocitame pomocnePole[i] pre i = 1 k dlzkaSearch-1
    while(i < dlzkaSearch)
    {
       if(search[i] == search[dlzka])
       {
         dlzka++;
         pomocnePole[i] = dlzka;
         i++;
       }
       else 			// (search[i] != search[dlzka])
       {
         if( dlzka != 0 )
         {
           dlzka = pomocnePole[dlzka-1];
         }
         else 			// if (dlzka == 0)
         {
           pomocnePole[i] = 0;
           i++;
         }
       }
    }
}

/*
 * sort - Zoradí znaky v danom retazci Stirng tak,
 *        aby znak s nižšou ordinálnou hodnotou vždy predchádzal znaku s vyššou ordinálnou hodnotou
 * @param char *string - zadany retezec
 * @return char * - vrati zoradený retazec podla ASCII
 */

char * sort (char *string)
{
    //1. Osetrenie pri predanom retazci == NULL;
    if(string == NULL)
    {
        return (char*)string;
    }
    int size = strlen(string);

    //2. Osetrenie pri predanom prazdnom rezazci == "";
    if(size == 0)
    {
        return (char*)string;
    }

    char *SortedString = malloc(size);
    if(SortedString == NULL)
    {
        InternalError("Internal error - Nie je k dispozicii dostatok pamate");
        return 0;
    }

    for(int i=0;i<size;)
    {
        SortedString[i] = string[i];
        i++;
        if(i==size)
        {
            SortedString[i]='\0';           //ukoncujem novy retazec so prazdnym znakom, aby to nezbloblo
        }
    }

    // Implementacia samotného Shell Sort -u
    int step,i,j,tmp;
    step = size/2;

    while (step > 0)
    {
        for (i = 1; i < size-1 ;i++)
        {
            j= i-step+1;
            while((j>=0) && (SortedString[j]>SortedString[j+step]))
            {
                tmp = SortedString[j];
                SortedString[j] = SortedString[j+step];
                SortedString[j+step] = tmp;
                j=j-step;
            }
        }
        step = step/2;
    }
    return (char*)SortedString;
}

/* hash funkcia */

unsigned int hFunc(const char *str, unsigned htab_size)
{
	unsigned int h=0;
	const unsigned char *p;

	for (p=(const unsigned char*)str; *p!='\0'; p++)
		h = 65599*h + *p;

	return h % htab_size;
}

/*inicializuje tabulku*/

Thtab_t *initTable(unsigned int size)
{
	Thtab_t *t = NULL;
	unsigned int i;

	if ((t = malloc(sizeof(Thtab_t) + size*sizeof(tSymbol *))) == NULL)
		return NULL;

	for (i = 0; i < size; i++)
		t->content[i] = NULL;

	t->htab_size = size;

	return t;
}

/* vrati pointer na vytvoreny symbol v tabulke */

tSymbol *h_Insert(Thtab_t *t, const char *key)
{
	unsigned int index;
	tSymbol *tmp = NULL, *tmp2 = NULL;
	bool found = false;

	index = hFunc(key, t->htab_size);

	if (t->content[index] == NULL)				// ak sa na danom indexe nenachadza ziadny symbol
	{
		tSymbol *newSymbol;

		if ((newSymbol = malloc((sizeof(tSymbol) + strlen(key) + 1))) == NULL)   // +1 kvoli nullbyte
			return NULL;

		strncpy(newSymbol->key, key, strlen(key));			  // kopia key do polozky
		newSymbol->next = NULL;

		t->content[index] = newSymbol;

		return newSymbol;
	}

	for (tmp = t->content[index]; tmp != NULL; tmp = tmp->next)			// hladanie key v zretazenom zozname
	{
		if (!strcmp(key,tmp->key))
		{
			found = true;
			break;
		}

		tmp2 = tmp;
	}

	if (found == false)
	{
		tSymbol *newSymbol;

		if ((newSymbol = malloc((sizeof(tSymbol) + strlen(key) + 1))) == NULL) // +1 kvoli nullbyte
			return NULL;

		strncpy(newSymbol->key, key, strlen(key));	// kopia key do polozky
		newSymbol->next = NULL;

		tmp2->next = newSymbol;
		return newSymbol;
	} else
		return NULL;
}

tSymbol *h_Lookup(Thtab_t *t, const char *key)
{
	unsigned int index;
	tSymbol *tmp = NULL;
	bool found = false;

	index = hFunc(key, t->htab_size);

	if (t->content[index] == NULL)				// ak sa na danom indexe nenachadza ziadny symbol
		return NULL;

	for (tmp = t->content[index]; tmp != NULL; tmp = tmp->next)			// hladanie key v zretazenom zozname
	{
		if (!strcmp(key,tmp->key))
		{
			found = true;
			break;
		}
	}

	if (found)					// najdeny symbol
		return tmp;
		else 					// ak nenajdeme
			return NULL;
}


/* uvolni tabulku */

void clearTable(Thtab_t *t)
{
	unsigned int i;
	tSymbol *tmp = NULL, *tmp2 = NULL;

    if (t == NULL) return;

	for (i = 0; i < t->htab_size; i++)
	{
		tmp = t->content[i];

		while (tmp != NULL)
		{
			tmp2 = tmp->next;
			free(tmp);
			tmp = tmp2;
		}

		t->content[i] = NULL;
	}
	if (t)
		free(t);

	return;
}

Thtab_t * copyTable(Thtab_t *t)
{
	int i;
	tSymbol *tmp;
	Thtab_t *copied = initTable(HSIZE);

	for (i = 0; i < HSIZE; i++)
	{

		for (tmp = t->content[i]; tmp != NULL; tmp = tmp->next)
		{

			tSymbol *newSymbol;

			if ((newSymbol = malloc((sizeof(tSymbol) + strlen(tmp->key) + 1))) == NULL) // +1 kvoli nullbyte
				return NULL;

			//COPY SYMBOL
			strncpy(newSymbol->key, tmp->key, strlen(tmp->key));	// kopia key do polozky
			newSymbol->next = copied->content[i];					// zaradenie na zaciatok
			newSymbol->type = tmp->type;
			newSymbol->funcOrVar = tmp->funcOrVar;
			newSymbol->state = tmp->state;
			newSymbol->value = tmp->value;

			copied->content[i] = newSymbol;
		}
	}

	return copied;
}
