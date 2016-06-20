// error.h
// Riesenie IJC-DU1, priklad b), 19.3.2014 
// Autor: Filip Mutnansky, FIT
// Prelozene: gcc 4.6.4 
// Popis: Obsahuje prototypy funkcii 
// pre vypis chyboveho hlasenia, pri-
// padne ukoncenia programu

#ifndef ERROR   // zabezpecenie proti viacnasobnemu definovaniu
#define ERROR

void Warning(const char *fmt, ...);
void FatalError(const char *fmt, ...);

#endif
