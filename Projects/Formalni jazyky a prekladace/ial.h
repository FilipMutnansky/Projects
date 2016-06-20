 // functions.h
 // Implementacia interpretu imperativneho jazyka IFJ14, 15.10.2014
 // Autor: Viktor Toth,xtothv01, FIT
 //        Peter Nagy,xnagyp01, FIT
 //        Dusan Zeliar,xzelia00, FIT
 //        Filip Mutnansky,xmutna00 , FIT
 //        Marek Ochodek,xochod01, FIT
 // Prelozeno: gcc 4.8.2
 // .h soubor ial.h
 // vestavene funkce

#ifndef IAL_H
#define IAL_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "scanner.h"

#define HSIZE 127
#define PARNUM 50

/* Struktura hashovacej tabulky */
typedef struct htab_t {			
  unsigned int htab_size;
  struct htab_t *next;
  struct symbol *content[HSIZE];
  struct symbol *params[PARNUM];
} Thtab_t;

/*zoznam instrukcii vo funkcii*/    
typedef struct
{
  struct listItem *First;
  struct listItem *Last; 
  struct listItem *Active;
} tListOfInstrFun;

/* Struktura symbolu v tabulke */
typedef struct symbol {
	char type;
	bool state;				  	   // 0 - neinicializovana, 1 - inicializovana
	bool funcOrVar;				   // 0 - funkcia, 1 - premenna
	bool defined;
	tListOfInstrFun *instructions;
	Thtab_t *funcTab;
	char* funcParams;
	tAttribute value;
	struct symbol *params[PARNUM];
	struct symbol *next;
	char key[];
} tSymbol;

/**
 * find - Vyhladá prvý vyskyt zadaného podretazca search v retazci string a vráti jeho pozicu (pocítane od jednicky)
 * @param char *string - zadany retezec
 * @param char *search - hladany podretazec
 * @return int - vrati první výskyt zadaného podretazca (poèítané od jednièky)
 */
int find(char *string, char *search);

/**
 * sort - Zoradí znaky v danom retazci Stirng tak,
 *        aby znak s ni¾¹ou ordinálnou hodnotou v¾dy predchádzal znaku s vy¹¹ou ordinálnou hodnotou
 * @param char *string - zadany retazec
 * @return char * - vrati zoradený retazec podla ASCII
 */
char * sort (char *String);

/* hash funkcia vracajuca index */
unsigned int hFunc(const char *str, unsigned htab_size);

/* inicializuje novu tabulku */
Thtab_t *initTable(unsigned int size);

/* vrati pointer na symbol v tabulke */
tSymbol *h_Insert(Thtab_t *t, const char *key);

tSymbol *h_Lookup(Thtab_t *t, const char *key);

/* uvolni tabulku */
void clearTable(Thtab_t *t);

/* vrati kopiu tabulky */
Thtab_t * copyTable(Thtab_t *t);
#endif
