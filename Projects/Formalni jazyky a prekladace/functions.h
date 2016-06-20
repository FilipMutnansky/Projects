 // functions.h
 // Implementacia interpretu imperativneho jazyka IFJ14, 15.10.2014
 // Autor: Viktor Toth,xtothv01, FIT
 //        Peter Nagy,xnagyp01, FIT
 //        Dusan Zeliar,xzelia00, FIT
 //        Filip Mutnansky,xmutna00 , FIT
 //        Marek Ochodek,xochod01, FIT
 // Prelozeno: gcc 4.8.2
 // .h soubor functions.h
 // vestavene funkce

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * copy - Vraci podretezec daneho retezce string
 * @param char *string - zadany podretezec
 * @param int first - zacatek pozadovaneho podretezce (pocitano od jednicky)
 * @param int count - delka podretezce
 * @return char * - vraci podretezec
 */
char *copy(char *string, int first, int count);



/**
 * copy - Vrátí dlzku (pocet znkaov) ·retazca
 * @param char *String - zadany retezec
 * @return char * - vraci cele cislo - integer
 */
int length (char *string);
