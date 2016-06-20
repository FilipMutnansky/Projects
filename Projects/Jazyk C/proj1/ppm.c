// ppm.c
// Riesenie IJC-DU1, priklad b), 19.3.2014 
// Autor: Filip Mutnansky, FIT
// Prelozene: gcc 4.6.4
// Popis: Obsahuje funkcie pre nacitanie a
// zapis do suboru vo formate ppm

#include <stdio.h>
#include <stdlib.h>
#include "ppm.h"
#include "error.h" //prepisat na h

#define MAX_COLOR 255
#define MAX_SIZE 1000*1000*3

struct ppm * ppm_read(const char * filename)
{
	char q,r;
	unsigned xsize,ysize,maxcolor;
	struct ppm *picture = NULL;
	FILE *f;
	
	if ((f = fopen(filename, "rb")) == NULL)      // otvorenie suboru
	{
		Warning("Chyba pri otvoreni suboru: %s\n",filename);
		return NULL;
	}
	
	if ((fscanf(f,"%c%c %u %u %u", &q,&r,&xsize,&ysize,&maxcolor)) < 5)  // nacitanie hlavicky
	{
		Warning("Chyba pri citani suboru: %s\n",filename);
		fclose(f);
		return NULL;
	}
	
	if (maxcolor != MAX_COLOR)      // overenie rozsahu farieb
	{
		Warning("Zly rozsah farieb v subore: %s\n",filename);
		fclose(f);
		return NULL;
	}
	
	if (xsize*ysize > MAX_SIZE)    // prekrocenie limitu
	{
		Warning("Velkost obrazka v subore %s presahuje maximum\n",filename);
		fclose(f);
		return NULL;
	}
	
	//alokacia struktury struct ppm
	if ((picture = (struct ppm*) malloc(sizeof(struct ppm)+3*sizeof(char)*xsize*ysize))==NULL)
	{
		Warning("Chyba pri alokovani struktury %s\n",filename);
		fclose(f);
		return NULL;
	}

    picture->xsize = xsize;    //priradenie velkosti do struct ppm
	picture->ysize = ysize;
		
	// nacitanie dat zo suboru ppm do struct ppm
	
	if ((fread(&picture->data,sizeof(char),3*sizeof(char)*xsize*ysize,f)) != 3*sizeof(char)*xsize*ysize)
	{
		Warning("Chyba pri citani suboru %s\n",filename);
		free(picture);
		fclose(f);
		return NULL;
	}
		
	fclose(f);
	return picture;
}

int ppm_write(struct ppm *p, const char * filename)
{
	FILE *f;
	
	if ((f = fopen(filename, "wb")) == NULL)  // otvorenie subrou na zapis
	{
		Warning("Chyba pri otvoreni suboru %s\n",filename);
		fclose(f);
		return -1;
	}
	
	if ((fprintf(f,"P6\n%u %u\n255",p->xsize,p->ysize))<0)   // zapis hlavicky
	{
		Warning("Chyba pri zapisovani do suboru %s\n",filename);
		fclose(f);
		return -1;
	}
	
	if (fwrite(p->data,sizeof(char),3*sizeof(char)*(p->xsize)*(p->ysize),f)) // zapis dat
	{
		Warning("Chyba pri zapisovani do suboru %s\n",filename);
		fclose(f);
		return -1;
	}	
	
	fclose(f);

	return 0;
}
