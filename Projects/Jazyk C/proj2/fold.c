// fold.c
// Riesenie IJC-DU2, priklad a), 13.4.2014 
// Autor: Filip Mutnansky, FIT
// Prelozene: gcc 4.6.4 
// Popis: Program zo zadaneho suboru cita
//		  text a formatuje ho parametrom -w 
//        na zadanu dlzku (impl. je 4096). 
//        Ak je zadany parameter -c zmeni 
//        maximalnu dlzku slova (80) na cislo
//		  zadane pri -w.

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>   // isspace
#include <stdlib.h>  // funkcia strtod
#include <string.h>  // funkcia strcmp

#define MAX_WORD_LENGTH 4096			// max dlzka slova
#define LINE_WIDTH 80					// impl. sirka textu

int getParams(int argc, char** argv, bool *cut, int *width, bool *file);		// funkcia spracuje argumenty prikazoveho riadku

int main(int argc, char** argv)
{
	bool isword = false;
	bool isspace = false;
	bool file = false;
	bool cut = false;
	unsigned int cutCount = 0;
	unsigned int charsOnLine = 0,space=1;
	unsigned int charsInWord = 1;
	unsigned int spaceCount = 0,i;
	unsigned int wordsOnLine = 0;
	unsigned int newLineCount = 0;
	unsigned int emptyLines = 0;
	unsigned int widthOfWord = MAX_WORD_LENGTH;
	char word[MAX_WORD_LENGTH];
	int c;
	int width = 0;
	
	if (argc - 1 != 0)	
	{
		if ((strcmp(argv[1],"-w") != 0) && ((strcmp(argv[1],"-c")) != 0))	// ak nie su zadane parametre -w -c nacitava zo stdin
		{				
			if (freopen(argv[1],"r", stdin) == NULL)						// presmeruje subor na stdin
				{
					fprintf(stderr,"CHYBA: Chybne zadane argumenty\n");
					return 1;
				} else 
					file = true;
		}		
	}
	
	if (getParams(argc, argv, &cut, &width, &file) == 1)				// spracuje argumenty
	{
		fprintf(stderr,"CHYBA: Chybne zadane argumenty\n");
		return 1;
	}
	
	if (!width)												// ak nie je zadane -w priradi impl. hodnotu
		width = LINE_WIDTH;							
	
	if (cut == false)
		widthOfWord = MAX_WORD_LENGTH;						// ak nie je zadane -c priradi implicitnu hodnotu
		else
			widthOfWord = width;
					
	word[0]=' ';						// nastavenie medzery pri vypise slova
		
	while ((c=getc(stdin)) != EOF)
	{		
		if (isspace(c))					// nenachadza sa v slove
		{
			isspace = true;
			isword = false;
		}
		
		if (!(isspace(c)))				// nachadza sa v slove
		{
			isspace = false;
			isword = true;
		}
		
		if (isword)					// ak narazi na slovo
		{
			spaceCount = 0;			
			word[charsInWord] = c;  // nacitanie do bufferu
			
			charsInWord++;
			charsOnLine++;		
		}
		
		if (isspace)				// ak narazi na oddelovac
		{
			spaceCount++;
						
			if (spaceCount == 1)	// spoji viac medzier do jednej
			{
				wordsOnLine++;	
				
				if (wordsOnLine == 1)		// ak je na riadku len jedno slovo neodmedzeruje
					space = 1;
					else
						space = 0;
						
				if (!space)							   // zaratanie medzery medzi znaky
					charsOnLine++;	
															
				if (charsOnLine > width)			   // prekrocenie sirky
				{
					if (wordsOnLine > 1) 			   // odriadkovanie (ak je pocet slov na riadku viac ako 1)
						printf("\n");
							      
					charsOnLine = charsInWord-1;	   // "prenesenie" poctu znakov na dalsi riadok
					space = 1;						   // neodmedzeruje
					wordsOnLine = 1;
				}
								
				for (i = space; i < ((widthOfWord+1 < charsInWord) ? widthOfWord+1 : charsInWord); i++)  // vypis slova (space == 1 -> bez medzery)
					printf("%c",word[i]);			   												 // (space == 0 -> s medzerou pred slovom)
								
				if (widthOfWord < charsInWord)									// sirka slova prekrocila limit -> vypis na stderr					
					cutCount++;													// zamedzenie viacnasobnym vypisom
								
			}
			charsInWord=1;
		}	
		
		if (newLineCount >= 2)		// odriadkuje pri dvoch '\n'
			{
				emptyLines++;
				
				if (emptyLines == 1)		// pri jednom prazdnom riadku
					printf("\n\n");
					else
						printf("\n");		// pri viac prazdnych riadkoch
				
				space = 1;
				charsOnLine = 0;
				wordsOnLine = 0;
			}
			
		
		if (c == '\n')					// sledovanie vyskytu kombinacie \n\n
			newLineCount++;
			else
				{
					newLineCount = 0;		
					emptyLines = 0;
				}
	}
	printf("\n");
	
	if (cutCount)
		fprintf(stderr,"WARNING: Doslo k oseknutiu slova\n");		// vypis na stderr
	
	return 0;	
}

int getParams(int argc, char **argv, bool *cut, int *width, bool *file)
{
	char *chyba;
	int i;
	unsigned int cutCount = 0, widthCount = 0;
	
	if (*file == true)
		i = 2;
			else
				i = 1;
	
	while (i <= (argc-1))
	{
		if ((strcmp(argv[i],"-w")) == 0)  // WIDTH
		{
			if ((i + 1) > (argc-1))
				return 1;				  // CHYBA
			
			i++;
			
			*width = strtol(argv[i],&chyba,10);			
			
			if (*chyba != 0 || *width <= 0)			// ak v cisle je nevhodny znak
				return 1;
			
			widthCount++;
			
		} else if ((strcmp(argv[i],"-c")) == 0)  // CUT
			{		
				*cut = true;
				cutCount++;
			} else 
				return 1;				
		
		i++;
		
		if (widthCount > 1 || cutCount > 1)			// viacnasobne zadane parametre
			return 1;
	}
		
	return 0;
}
