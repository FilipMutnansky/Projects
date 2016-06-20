// error.c
// Riesenie IJC-DU1, priklad b), 19.3.2014 
// Autor: Filip Mutnansky, FIT
// Prelozene: gcc 4.6.4 
// Popis: Obsahuje definicie funkcii s pre-
// mennym poctom parametrov, pre vypis chy-
// boveho hlasenia, pripadne ukoncenia programu

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "error.h"

void Warning(const char *fmt, ...)
{
	va_list args;       // praca so zasobnikom
	
	va_start(args, fmt);
	
	fprintf(stderr, "CHYBA: ");		// vypise sa vzdy
	vfprintf(stderr,fmt,args);		// vypis fmt s premennym poctom arg.
	
	va_end(args);
	return;
}

void FatalError(const char *fmt, ...)
{
	va_list args;			// praca so zasobnikom
	
	va_start(args, fmt);
	
	fprintf(stderr, "CHYBA: ");		// vypise sa vzdy
	vfprintf(stderr,fmt,args);		// vypis fmt s premennym poctom arg.
	
	va_end(args);
	
	exit(1);
}
