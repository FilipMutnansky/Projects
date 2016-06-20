 // error.h
 // Implementacia interpretu imperativneho jazyka IFJ14, 15.10.2014
 // Autor: Viktor Toth,xtothv01, FIT
 //        Peter Nagy,xnagyp01, FIT
 //        Dusan Zeliar,xzelia00, FIT
 //        Filip Mutnansky,xmutna00 , FIT
 //        Marek Ochodek,xochod01, FIT
 // Prelozene: gcc 4.8.2
 // .c subor prekladac.c
 // kniznica error.h


#ifndef ERROR_H
#define ERROR_H
//======================== FILES INCLUDED ===============================
#include <stdbool.h>
#include <stdio.h>
//======================== GLOBAL VARIABLES ===============================
extern bool errflag;
extern int errnumber;
extern  FILE* source_file;  

//======================== FUNCTION PROTOTYPES ===============================
void Warning(const char *fmt, ...);

void ErrorPrint(const char *fmt, ...);
		
void ScannerError(const char *fmt, ...);	

void InternalError(const char *fmt, ...);

void ParseExpressionError(const char *fmt, ...);

void CompatibilityError(const char *fmt, ...);

void InterpretReadError(const char *fmt, ...);

void ZeroDiv();

void Uninitialized();

#endif

