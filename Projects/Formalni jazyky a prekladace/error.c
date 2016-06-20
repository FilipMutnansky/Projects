 // error.c
 // Implementacia interpretu imperativneho jazyka IFJ14, 15.10.2014
 // Autor: Viktor Toth,xtothv01, FIT
 //        Peter Nagy,xnagyp01, FIT
 //        Dusan Zeliar,xzelia00, FIT
 //        Filip Mutnansky,xmutna00 , FIT
 //        Marek Ochodek,xochod01, FIT
 // Prelozene: gcc 4.8.2
 // .c subor prekladac.c
 // implementacia funkcii osetrujucich vyskytnute chyby v programe

#include <stdio.h>
#include <stdarg.h>
#include "error.h"
#include <stdlib.h>

//======================== GLOBAL VARIABLES ===============================
bool errflag=false;
int errnumber;

//======================== FUNCTIONS ===============================
//prints to stderr
void Warning(const char *fmt, ...){
	va_list args;
	va_start(args,fmt);

	fprintf( stderr, "CHYBA: " );
    vfprintf( stderr, fmt, args );
    va_end( args );
    return;
}

//prints to stderr, call exit
//1 - chyba v programu v r�mci lexik�ln� anal�zy (chybn� struktura aktu�ln�ho lex�mu).
void ScannerError(const char *fmt, ...){
	va_list args;
	va_start(args,fmt);

	fprintf( stderr, "CHYBA: " );
    vfprintf( stderr, fmt, args );
    va_end( args );

    errflag = true;
    errnumber = 1;

    return;
}

//prints to stderr, returns control
void ErrorPrint(const char *fmt, ...){
	va_list args;
	va_start(args,fmt);

    vfprintf( stderr, fmt, args );
    va_end( args );
    return;
}
//99 - intern� chyba interpretu tj. neovlivnen� vstupn�m programem (napr. chyba alokace pameti,
        //chyba pri otv�r�n� souboru s r�dic�m programem, �patn� parametry pr�kazov� r�dky atd.).
void InternalError(const char *fmt, ...){
	va_list args;
	va_start(args,fmt);

	fprintf( stderr, "CHYBA: " );
    vfprintf( stderr, fmt, args );
    va_end( args );

    errflag = true;
    errnumber = 99;

    return;
}
//2 - chyba v programu v r�mci syntaktick� anal�zy (chybn� syntaxe struktury programu).
void ParseExpressionError(const char *fmt, ...){
	va_list args;
	va_start(args,fmt);

	fprintf( stderr, "CHYBA: " );
    vfprintf( stderr, fmt, args );
    va_end( args );

    errflag = true;
    errnumber = 2;

    return;
}
//4 - s�mantick� chyba typov� kompatibility v aritmetick�ch, retezcov�ch a relacn�chv�razech, pr�p. �patn� pocet ci typ parametru u vol�n� funkce.
void CompatibilityError(const char *fmt, ...){
    va_list args;
    va_start(args,fmt);

    fprintf( stderr, "CHYBA: " );
    vfprintf( stderr, fmt, args );
    va_end( args );

    errflag = true;
    errnumber = 4;

    return;
}
//6 - behov� chyba pri nac�t�n� c�seln� hodnoty ze vstupu.
void InterpretReadError(const char *fmt, ...){
    va_list args;
    va_start(args,fmt);

    fprintf( stderr, "CHYBA: " );
    vfprintf( stderr, fmt, args );
    va_end( args );

    errflag = true;
    errnumber = 6;

    return;
}
//8 - behov� chyba delen� nulou.
void ZeroDiv()
{
	fprintf( stderr, "CHYBA: Division by zero\n" );

	errflag = true;
	errnumber = 8;

    return;
}
//7 - behov� chyba pri pr�ci s neinicializovanou promennou.
void Uninitialized()
{
	fprintf( stderr, "CHYBA: Uninitialized variable\n" );

	errflag = true;
	errnumber = 7;

    return;
}
