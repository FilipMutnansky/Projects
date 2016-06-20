// io.c
// Riesenie IJC-DU2, priklad b), 13.4.2014 
// Autor: Filip Mutnansky, FIT
// Prelozene: gcc 4.6.4 
// Popis: Obsahuje funkciu fgetw, ktora
//		  nacita jedno slovo zo zadaneho
//	      suboru a vrati pocet nacitanych
//		  znakov (v pripade oseknutia 
//		  na max-1 znakov vrati max)

#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include "htab.h"

static int fgetw(char *s, int max, FILE *f);

static int fgetw(char *s, int max, FILE *f)
{
	int c;
	unsigned int i = 0;
	bool cut = false;
	
	if ((s == NULL) || (f == NULL) || (max < 1)) 	// kontrola vstupnych argumentov funkcie
		return EOF;
	
	if (max > LIMIT)		// ak je max vacsie ako impl. LIMIT
		max = LIMIT;
	
	while ((c = fgetc(f)) != EOF)
	{	
		if (!isspace(c))    // ak je znak
		{
			if (i < max-1)
			{
				s[i] = c;
				i++;
			} else
				cut = true;
			
		} else              // ak je oddelovaci znak 
			{
				if (i != 0)		// ak nie je medzera na zaciatku -> break
					break;
			}	
	}
	
	s[i] = '\0';
	
	if (c == EOF)
		return EOF;
		else if (cut == true)		// ak bolo orezane slovo, vrat max
			return max;
			else
				return i;
}
