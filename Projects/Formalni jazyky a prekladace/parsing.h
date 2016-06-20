 // parsing.h
 // Implementacia interpretu imperativneho jazyka IFJ14, 15.10.2014
 // Autor: Viktor Toth,xtothv01, FIT
 //        Peter Nagy,xnagyp01, FIT
 //        Dusan Zeliar,xzelia00, FIT
 //        Filip Mutnansky,xmutna00 , FIT
 //        Marek Ochodek,xochod01, FIT
 // Prelozene: gcc 4.8.2
 // .h subor parsing.h
 // header subor pre pracu syntaktickej analyzi
 
#ifndef PARSING_H
#define PARSING_H  

//======================== FILES INCLUDED ===============================
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include "scanner.h"
#include "error.h"
#include "ial.h"
#include "sem.h"


//======================== GLOBAL VARIABLES ===============================

extern FILE * source_file;
extern TStack_tab *s;
extern tToken act_token;
extern tListOfInstr *list;
extern char expType;
extern TStack_val *stack;
extern Thtab_t *globTable ;
extern char *parTypes;
extern bool inFunction;
extern char *actFunction;

/* hodnota a typ terminalu */
typedef struct terminfo {
  tAttribute val;
  char typ;
  bool varOrCon;         // var 0 , con 1
  char *id;
} tTermInfo;

typedef struct tStackItem {
    struct tStackItem * DownPtr;
    struct tStackItem * UpPtr;
    char symbol;
    tTermInfo info;
} *tStackItemPtr;	

typedef struct {
	tStackItemPtr Top;
	tStackItemPtr Terminal;
} tStack;
//======================== FUNCTION PROTOTYPES ===============================

void ErrorPrint(const char *fmt, ...);
bool isTerm();
bool first(int pocet, ... );
bool parserError(int pocet, ... );
bool expect (tTypSymbolu exp_typ);
const char * enumToString (tTypSymbolu exp_typ);
bool SemError();
bool SemError2();

bool prog();
bool dpre();
bool dfun();
bool mblo();
bool prem();
bool typ();
bool e();
bool dfun();
bool hlav();
bool q();
bool zpar ();
bool f ();
bool fblo ();
bool blok ();
bool zpri ();
bool g ();
bool prik ();
bool ifpo();
bool h();
bool vypi();
bool j();
bool vyraz ();
bool funvyraz();
bool parameter();
bool k();

void InitStack (tStack *S);
void DisposeStack (tStack *S);
bool PushTop (tStack *S, char znak );
bool PopTop (tStack *S, char * znak);
bool InsertStartSymbol (tStack *S);
bool FindTopTerminal(tStack *S);
int reduce(tStack *S);
int charToIndex (char znak);
char tokenToChar ();
bool parseExpression ();

tSymbol *findParam(char *name);
tSymbol *findParamSem(char *name);
tSymbol *findSymbolInFun(char *name);
tSymbol *findSymbolInFunSem(char *name);
tSymbol **copyParams(tSymbol **params);
bool checkForward();

bool detectTerminaType(char *idTyp, tAttribute* pointerToData);
bool createWriteInstuction();
bool createWriteInstuctionFun();
bool createReadlnInstruction(char * id);
bool createReadlnInstructionFun(char * id);

void generateInstructionFun(tListOfInstrFun *listFun, int instType, void *addr1, void *addr2, void *addr3);
tInstr *listGetDataFun(tListOfInstrFun *L);
void listNextFun(tListOfInstrFun *L);
void listPreviousFun(tListOfInstrFun *L);
void listFirstFun(tListOfInstrFun *L);
void listLastFun(tListOfInstrFun *L);
void *listGetPointerLastFun(tListOfInstrFun *L);
int listInsertLastFun(tListOfInstrFun *L, tInstr I);
void listFreeFun(tListOfInstrFun *L);
void listInitFun(tListOfInstrFun *L);
int listInsertNextFun(tListOfInstrFun *L, tInstr I);






#endif
