// ppm.h
// Riesenie IJC-DU1, priklad b), 19.3.2014 
// Autor: Filip Mutnansky, FIT
// Prelozene: gcc 4.6.4
// Popis: Obsahuje prototypy funkcii pre
// nacitanie a zapis do suboru vo formate ppm

#ifndef PPM      // zabezpecenie proti viacnasobnemu definovaniu
#define PPM

//struktura do ktorej sa nacita ppm subor
struct ppm {         
	unsigned xsize; // pocet pixelov na sirku
	unsigned ysize; // pocet pixelov na vysku
	char data[];    // jednotlive bajty (RGB)
	};

struct ppm * ppm_read(const char * filename);

int ppm_write(struct ppm *p, const char * filename);


#endif
