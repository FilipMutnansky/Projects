// scanner.h
 // Implementacia interpretu imperativneho jazyka IFJ14, 15.10.2014
 // Autor: Viktor Toth,xtothv01, FIT
 //        Peter Nagy,xnagyp01, FIT
 //        Dusan Zeliar,xzelia00, FIT
 //        Filip Mutnansky,xmutna00 , FIT
 //        Marek Ochodek,xochod01, FIT
 // Prelozene: gcc 4.8.2
 // .h subor scanner.h
 // header subor definujuci datove typy a prototypy scanneru
 
#ifndef SCANNER_H
#define SCANNER_H

//======================== FILES INCLUDED ===============================
#include <stdbool.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"

//======================== DATA STRUCTURES ===============================


/* Union ktory obsahuje struktura, definuje atribut tokenu*/
typedef union {
  int integer;
  double real;
  /*const*/ char *string; 
  bool boolean;
} tAttribute;

/* Enum tabulka pouzivanych symbolov*/
typedef enum {
  GENERAL,                        /* 0: identifikator */
  INTEGER,                        /* 1: cele cislo */
  REAL,                           /* 2: desatinne cislo */
  END,                            /* 3: konec suboru */
  STRING,                         /* 4: retazec */
  /*OTHER,                           5: nedefinovane symboli, napr. @^ */

  OPERATOR_ASSIGN,                /* 5: := */
  OPERATOR_COLON,                 /* 6: : */
  OPERATOR_NOT_EQUAL,             /* 7: <> */
  OPERATOR_LESS,                  /* 8: < */
  OPERATOR_LESS_EQUAL,            /* 9: <= */
  OPERATOR_GREATER,               /* 10: > */
  OPERATOR_GREATER_EQUAL,         /* 11: >= */
  OPERATOR_EQUAL,                 /* 12: = */
  OPERATOR_SEMICOLON,             /* 13: ; */
  OPERATOR_MUL,                   /* 14: * */
  OPERATOR_DIV,                   /* 15: / */
  OPERATOR_PLUS,                  /* 16: + */
  OPERATOR_MINUS,                 /* 17: - */
  OPERATOR_BRACKET_OPEN,          /* 18: ( */
  OPERATOR_BRACKET_CLOSE,         /* 19: ) */
  OPERATOR_CURLY_BRACKET_OPEN,    /* 20: { */
  OPERATOR_CURLY_BRACKET_CLOSE,   /* 21: } */
  OPERATOR_COMM,                  /* 22: , */
  OPERATOR_DOT,                   /* 23: . */
  
  RES_BEGIN,
  RES_BOOL,
  RES_DO,
  RES_ELSE,
  RES_END,
  RES_FALSE,
  RES_FIND,
  RES_FORWARD,
  RES_FUNCTION,
  RES_IF,
  RES_INTEGER,
  RES_READLN,
  RES_REAL,
  RES_SORT,
  RES_STRING,
  RES_THEN,
  RES_TRUE,
  RES_VAR,
  RES_WHILE,
  RES_WRITE,
  // rozsirenie
  RES_REPEAT,
  RES_UNTIL
}tTypSymbolu;

/* Struktura pomocou ktorej scanner preda token */ 
typedef struct {
    tTypSymbolu typ;
    tAttribute attribute;
}tToken;
 
//======================== FUNCTION PROTOTYPES ===============================

/* vrati nasledujuci nacitany token cez * token */
bool getNextToken (FILE* file, tToken * token);

/* vrati nasledujuci nacitany token cez * token bez posunutia v subore */
bool lookNextToken(FILE* file, tToken * token); 

#endif
