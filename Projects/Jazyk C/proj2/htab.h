// htab.h
// Riesenie IJC-DU2, priklad b), 13.4.2014 
// Autor: Filip Mutnansky, FIT
// Prelozene: gcc 4.6.4 
// Popis: Hlavickovy subor pre priklad b)

#ifndef HTAB
#define HTAB

#define LIMIT 127

typedef struct htab_listitem {		// struktura pre prvok v lin. zozname hash tabulka
  unsigned int data;                // pocet vyskytov
  struct htab_listitem *next; 		// ukazatel na naslednujuci prvok
  char key[];               	    // meno (flexible array member)
} Thtab_listitem;


typedef struct htab_t {				// struktura hashovacej tabulky
  unsigned int htab_size;			// velkost pola ukazatelov
  Thtab_listitem *content[];        // zoznam (flexible array member)
} Thtab_t;

// Prototypy funkcii

unsigned int hfunc(const char *str, unsigned htab_size);
Thtab_t* htab_init(unsigned int size);
void htab_free(Thtab_t *t);
void htab_clear(Thtab_t *t);
void htab_statistics(Thtab_t *t);
void htab_remove(Thtab_t *t,const char *key);
void htab_foreach(Thtab_t *t, void (*function)(const char *key, unsigned int value));
Thtab_listitem *htab_lookup(Thtab_t *t, const char *key);
void htab_print(const char *key, unsigned int value);

#endif
