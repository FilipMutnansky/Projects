 // functions.c
 // Implementacia interpretu imperativneho jazyka IFJ14, 15.10.2014
 // Autor: Viktor Toth,xtothv01, FIT
 //        Peter Nagy,xnagyp01, FIT
 //        Dusan Zeliar,xzelia00, FIT
 //        Filip Mutnansky,xmutna00 , FIT
 //        Marek Ochodek,xochod01, FIT
 // Prelozene: gcc 4.8.2
 // .c subor functions.c
 // vestavene funkce : copy, length

#include "functions.h"
#include "error.h"
/*
 * copy - Vraci podretezec daneho retezce string
 * @param char *string - zadany podretezec
 * @param int first - zacatek pozadovaneho podretezce (pocitano od jednicky)
 * @param int count - delka podretezce
 * @return char * - vraci podretezec
 *
 * Poznamka:
 *  - error u mallocu, je to OK?
 */
char *copy(char *string, int first, int count)
{
  char* newString;

  if(count <= 0 || string[0] == '\0')
    return "";                            // pokud zada prazdny retezec nebo count<=0, vracim prazdny retezec

  if(first < 1)                           // pocatek vzdy musi byt na pozici 1
    first=1;

  if((strlen(string)+1 - (unsigned)first) < (unsigned)count)  // pro zbytecnou alokaci pameti
    count = strlen(string)+1 - first;

  newString = (char*)malloc(count+1);
  if (newString == NULL) {
    InternalError("Internal error - Nie je k dispozicii dostatok pamate");
    return NULL;                          // ERROR RETURN !!!!! - nedostatek pameti
  }
  strncpy(newString, string+first-1, count);
  newString[count] = '\0';
  return newString;
}

/*
 * copy - Vrati dlzku (pocet znkaov) retazca
 * @param char *string - zadany retezec
 * @return char * - vraci cele cislo - integer
 */
int length (char *string)
{
    if(string == NULL)
    {
        return 0;
    }
    int size = strlen(string);
    return size;
}
