// steg-decode.c
// Riesenie IJC-DU1, priklad b), 19.3.2014 
// Autor: Filip Mutnansky, FIT
// Prelozene: gcc 4.6.4
// Popis: Dekoduje spravu v obrazku
// vo formate ppm

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "ppm.h"
#include "eratosthenes.c"
#include "bit-array.h"


#define LIMIT 1000*1000*3
#define BYTE 8 

int main(int argc, char *argv[])
{
	if (argc == 2)				// ak nie je zadany chybny pocet argumentov
	{
		struct ppm *picture = ppm_read(argv[1]);  // nacitanie struktury
		char sprava = 0,index2=0,bit;
		unsigned long index;
		int nullbyte = 0;
		
		if (picture == NULL) 			// ak nacitanie skoncilo s chybou
			FatalError("Chybne zadany subor\n");
		
		BitArray(bitmap,LIMIT);   // iniciaizacia bit. pola
	
		Eratosthenes(bitmap);   // vypocet prvociselnych indexov
			
		for (index=2;index <= (LIMIT); index++)	
		{
			if (GetBit(bitmap,index) == 0)   // prvociselny index
			{	
				bit = DU1_GET_BIT((&(picture->data)[index+1]),0); // vyber LSB z bytu
				DU1_SET_BIT((&sprava),index2,bit);				  // nastav na nasledujuci index vybrany bit
												
				if (index2 < (BYTE - 1))							  // ci uz nie je zaplneny cely byte
				{
					index2++;
				} 
					else if ((sprava != '\0') && (isprint(sprava)))   // ak nie je znak nulovy alebo netlacitelny
					{
						printf("%c",sprava);						  // vytlaci 1 znak zo spravy
						index2 = 0; 								  // vynulue pocet nast. bitov a znak
						sprava = 0;
					}
						else if (sprava == '\0')					 // ak je znak nulovy -> koniec spravy
						{
							printf("\n");
							nullbyte = 1;
							break;
						}
						else                						// ak je znak netlacitelny -> fatal error
							{									
								free(picture);
								FatalError("Sprava ulozena v subore: %s nie je korektna\n",argv[1]);
							}
			}		
		}
		
	if (nullbyte == 0)
	{
		free(picture);
		FatalError("Sprava ulozena v subore: %s nie je korektne ukoncena\n",argv[1]);
		return 1;
	} else
		{
			free(picture);													// uvolnenie alokovanej pamate
			return 0;
		}
		
	} else
		{
			FatalError("Nespravne zadane argumenty\n");
			return 1;
		}
	
	
}
