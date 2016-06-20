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
//1 - chyba v programu v rámci lexikální analýzy (chybná struktura aktuálního lexému).
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
//99 - interní chyba interpretu tj. neovlivnená vstupním programem (napr. chyba alokace pameti,
        //chyba pri otvírání souboru s rídicím programem, ¹patné parametry príkazové rádky atd.).
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
//2 - chyba v programu v rámci syntaktické analýzy (chybná syntaxe struktury programu).
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
//4 - sémantická chyba typové kompatibility v aritmetických, retezcových a relacníchvýrazech, príp. ¹patný pocet ci typ parametru u volání funkce.
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
//6 - behová chyba pri nacítání císelné hodnoty ze vstupu.
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
//8 - behová chyba delení nulou.
void ZeroDiv()
{
	fprintf( stderr, "CHYBA: Division by zero\n" );

	errflag = true;
	errnumber = 8;

    return;
}
//7 - behová chyba pri práci s neinicializovanou promennou.
void Uninitialized()
{
	fprintf( stderr, "CHYBA: Uninitialized variable\n" );

	errflag = true;
	errnumber = 7;

    return;
}
