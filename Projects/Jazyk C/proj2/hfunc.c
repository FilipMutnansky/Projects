// hfunc.c
// Riesenie IJC-DU2, priklad b), 13.4.2014 
// Autor: Filip Mutnansky, FIT
// Prelozene: gcc 4.6.4 
// Popis: Hashovacia funkcia na ziskanie
//  	  indexu to tabulky podla kluca.

#include <stdio.h>
#include "htab.h"

//                        kluc			velkost
unsigned int hfunc(const char *str, unsigned htab_size) 
{ 
	unsigned int h=0; 
	const unsigned char *p; 
	
	for (p=(const unsigned char*)str; *p!='\0'; p++) 
		h = 65599*h + *p; 
		
	return h % htab_size; 
}
