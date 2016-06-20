// wordcount.c
// Riesenie IJC-DU2, priklad b), 13.4.2014 
// Autor: Filip Mutnansky, FIT
// Prelozene: gcc 4.6.4 
// Popis: Vypise pomocou hashovacej tabulky
// 		  pocet vyskytov slov v texte

#include <stdio.h>
#include <stdbool.h>
#include "htab.h"
#include "io.c"

// Velkost tabulky ovplyvnuje priemernu dlzku zoznamu v tabulke,
// ak sa velkost znizuje priemerna hodnota sa zvysuje, ak sa
// naopak zvysuje priemerna dlzka zoznamu sa znizuje 
// (vela prazdnych poloziek). Pri dlhych textoch je vhodne zvolit
// velkost vacsiu, pri kratsich je vhodne zvolit nizsiu velkost.
// Pri mojom testovani programu som pouzival texty obsahujuce
// okolo 800 slov a pri velkosti 512 sa priemerna dlzka zoznamu 
// pohybovala okolo 0.8. 

#define HTAB_SIZE 512

void htab_print(const char *key, unsigned int value);

int main()
{
	Thtab_t* t = NULL;
	bool cut = false;
	unsigned int wordLength = 0;
	char word[LIMIT];											// LIMIT = 127
	
	t=htab_init(HTAB_SIZE);										// inicializacia tabulky
		
	while ((wordLength = fgetw(word, LIMIT, stdin)) != EOF)		// nacitava az po koniec suboru
	{
		if ((htab_lookup(t, word)) == NULL)						// slovo s *word prida do hash. tabulky
		{
			fprintf(stderr, "CHYBA: Chyba pri alokacii\n");
			return 1;
		}
		
		if (wordLength == LIMIT)							// ak je vratena dlzka slova rovna max nastavi oseknutie
			cut = true;		
	}
	
	if (cut == true)											// ak bolo nastavene cut aspon raz vypise sa warning na stderr
		fprintf(stderr, "WARNING: Slovo bolo oseknute\n");
	
	htab_foreach(t, htab_print);							// vypis tabulky
	htab_free(t);											// uvolnenie tabulky
	
	return 0;
}

void htab_print(const char *key, unsigned int value)
{
  printf("%s\t%u\n", key, value);							// vypis tabulky v tvare key\tdata
  return;
}
