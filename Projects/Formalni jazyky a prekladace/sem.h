 // sem.h
 // Implementacia interpretu imperativneho jazyka IFJ14, 15.10.2014
 // Autor: Viktor Toth,xtothv01, FIT
 //        Peter Nagy,xnagyp01, FIT
 //        Dusan Zeliar,xzelia00, FIT
 //        Filip Mutnansky,xmutna00 , FIT
 //        Marek Ochodek,xochod01, FIT
 // Prelozene: gcc 4.8.2
 // .h subor scanner.h
 // header subor definujuci datove typy a prototypy sem. analyzy
 
#ifndef SEM_H
#define SEM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "ial.h"
#include "scanner.h"

/* Zasobnik tabuliek (bloky) */
typedef struct Stack_tab {
  Thtab_t *STop;
} TStack_tab;

/* hodnota vo vyraze */
typedef struct expVal {
  tAttribute val;
  char typ;
  bool varOrCon;         // var 0 , con 1
  char *id;
  struct expVal *next;
} texpVal;

/* Zasobnik hodnot */
typedef struct Stack_val {
  texpVal *STop;
} TStack_val;

/*instrukcia*/
typedef struct
{
  int instType;
  void *addr1; 
  void *addr2; 
  void *addr3;
} tInstr;

/*prvok zoznamu*/
typedef struct listItem
{
  tInstr Instr;
  struct listItem *next;
  struct listItem *prev;
} tListItem;

/*zoznam instrukcii*/    
typedef struct
{
  struct listItem *First;
  struct listItem *Last; 
  struct listItem *Active;
} tListOfInstr;

void Stack_init(TStack_tab *s); 
int Stack_push(Thtab_t *tab);
void Stack_pop();

void Stack2_init(TStack_val *s); 
int Stack2_push(TStack_val *s, texpVal *val);
void Stack2_pop(TStack_val *s);
tSymbol *lookInTables(const char *key);
tSymbol *lookInTablesSem(const char *key);

char *addCharToString(char *s, char c);

tInstr *listGetData(tListOfInstr *L);
void *listGetPointerLast(tListOfInstr *L);
void listNext(tListOfInstr *L);
void listPrevious(tListOfInstr *L);
void listFirst(tListOfInstr *L);
void listLast(tListOfInstr *L);
int listInsertLast(tListOfInstr *L, tInstr I);
void listFree(tListOfInstr *L);
void listInit(tListOfInstr *L);
int listInsertNext(tListOfInstr *L, tInstr I);

bool semCheckVariable();

void generateInstruction(tListOfInstr *list, int instType, void *addr1, void *addr2, void *addr3);
#endif

