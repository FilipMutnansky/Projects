 // interpret.h
 // Implementacia interpretu imperativneho jazyka IFJ14, 15.10.2014
 // Autor: Viktor Toth,xtothv01, FIT
 //        Peter Nagy,xnagyp01, FIT
 //        Dusan Zeliar,xzelia00, FIT
 //        Filip Mutnansky,xmutna00 , FIT
 //        Marek Ochodek,xochod01, FIT
 // Prelozene: gcc 4.8.2
 // .h subor scanner.h
 // header subor interpretu

#ifndef INTERPRET_H
#define INTERPRET_H

extern int numOfLabels;

#include "scanner.h"
#include "parsing.h"
#include <stdbool.h>
#include "error.h"
#include "ial.h"
#include "sem.h" 
 
#define I_PUSHVAL 0
#define I_PLUS 1
#define I_MUL 2
#define I_DIV 3
#define I_SUB 4
#define I_WRITE_BOOL 5
#define I_WRITE_INT 6
#define I_WRITE_REAL 7
#define I_WRITE_STRING 8
#define I_READLN_BOOL 9 
#define I_READLN_INT 10
#define I_READLN_REAL 11
#define I_READLN_STRING 12
#define I_ASSIGN 13
#define I_GREATER 14
#define I_LOWER 15
#define I_GREATER_EQUAL 16
#define I_LOWER_EQUAL 17
#define I_EQUAL 18
#define I_NOT_EQUAL 19
#define I_GO_OR_STAY 20
#define I_LAB_OR_GO 21
#define I_LAB_OR_NO 22
#define I_GOTO_LAB 23
#define I_LAB 24
#define I_POP_STACK 25
#define I_GO_OR_STAY2 26
#define I_GO_BACK_OR_STAY 27
#define I_GO_BACK_TO_LAB 28
#define I_PUSHPAR_INT 29
#define I_PUSHPAR_REAL 30
#define I_PUSHPAR_BOOL 31
#define I_PUSHPAR_STRING 32
#define I_CALL_FUN 33
#define I_PUSH_TABLE 34
#define I_EVAL 35
#define I_POP_TABLE 36
#define I_REVERSE_BOOL 37
#define I_LENGTH 38
#define I_COPY 39
#define I_FIND 40
#define I_SORT 41


int interpret(tListOfInstr *list);
int interpretFun(char *name);
int pushValueI (texpVal *source);
int plus();
int mul();
int divide();
int sub();
char * concatenate(char * s1, char * s2);
void writeBool(tAttribute * pointerToData, bool *pointerToState);
void writeInt(tAttribute * pointerToData, bool *pointerToState);
void writeReal(tAttribute * pointerToData, bool *pointerToState);
void writeString(tAttribute * pointerToData, bool *pointerToState);
void readlnInt(tAttribute * pointerToData);
void readlnReal(tAttribute * pointerToData);
void readlnString(tAttribute * pointerToData);
int assign(char *s);
int greater();
int greaterOrEqual();
int lower();
int lowerOrEqual();
int equal();
int notEqual();
char *generateLabelName();
int eval();
int goOrStay(char *name);
int goToLab();
int goBackToLab(char *name);
int popStack();
int goOrStay2(char *name);	
int goBackOrStay(char *name);
void generateInstructionNext(tListOfInstr *list, int instType, void *addr1, void *addr2, void *addr3);
void generateInstructionNextFun(tListOfInstrFun *listFun, int instType, void *addr1, void *addr2, void *addr3);
int pushParString(tSymbol *dest, char *source, char *val);
int pushParBool(tSymbol *dest, char *source, bool *val);
int pushParReal(tSymbol *dest, char *source, double *val);
int pushParInt(tSymbol *dest, char *source, int *val);
int pushTable(Thtab_t *tab, tSymbol **par);
void ReverseBool();
int importFunctions();
int callLength();
int callCopy();
int callFind();
int callSort();
//void callFun(tListOfInstrFun* instrList);


#endif



