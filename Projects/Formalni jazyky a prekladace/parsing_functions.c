 // parsing_functions.c
 // Implementacia interpretu imperativneho jazyka IFJ14, 15.10.2014
 // Autor: Viktor Toth,xtothv01, FIT
 //        Peter Nagy,xnagyp01, FIT
 //        Dusan Zeliar,xzelia00, FIT
 //        Filip Mutnansky,xmutna00 , FIT
 //        Marek Ochodek,xochod01, FIT
 // Prelozene: gcc 4.8.2
 // .c subor parsing_functions.c
 // subor obsahuje funkcie volane parserom

//======================== FILES INCLUDED ===============================
#include "parsing.h"
#include "scanner.h"
#include "error.h"

//======================== FUNCTIONS ===============================

bool isTerm () {
	if (act_token.typ == GENERAL || act_token.typ == INTEGER || act_token.typ == REAL ||
		 act_token.typ == STRING  || act_token.typ == RES_TRUE || act_token.typ == RES_FALSE ) {
		if (act_token.typ == END) return true;
		else return (getNextToken ( source_file, &act_token));
	}
	else {
		parserError(4,GENERAL,INTEGER,REAL,STRING);
		return false;
	}
	return true;
}

bool expect (tTypSymbolu exp_typ) {
	
	if (act_token.typ == exp_typ) {
		if (act_token.typ == END) return true;
		else return (getNextToken ( source_file, &act_token));
	}
	else {
		parserError(1,exp_typ);
		return false;
	}
	return true;
}	

bool first(int pocet, ... ) {
	va_list list;
	va_start(list,pocet);
	for (int i = 0; i < pocet; i++){

		if (act_token.typ == va_arg(list, tTypSymbolu)){
			va_end( list );
			return true;
		}
	}
	va_end( list );
	return false;
}

bool parserError(int pocet, ... ) {
	va_list list;
	va_start(list,pocet);
	
	Warning("Expected '");
	for (int i = 0; i < (pocet); i++){
		if ( i!= 0) ErrorPrint("' or '");
		ErrorPrint( enumToString(va_arg(list, tTypSymbolu)) );
	}
	ErrorPrint("' but '");
	ErrorPrint( enumToString( act_token.typ ) );
	ErrorPrint("' found\n");

	va_end( list );
	
	errflag= true;
	errnumber= 2;
	return false ;
}

bool SemError() {
	ErrorPrint("Undefined function or variable, or multiple definition of variable or function\n");
	errflag= true;
	errnumber= 3;
	return false ;
}

bool SemError2() {
	ErrorPrint("Mismatching types\n");
	errflag= true;
	errnumber= 4;
	return false ;
}

const char * enumToString (tTypSymbolu exp_typ) {
  
    if (exp_typ == RES_BEGIN) return "begin";
    else if (exp_typ == RES_BOOL) return "bool";
    else if (exp_typ == RES_DO) return "do";
    else if (exp_typ == RES_ELSE) return "else";
    else if (exp_typ == RES_END) return "end";
    else if (exp_typ == RES_FALSE) return "false";
    else if (exp_typ == RES_FIND) return "find";
    else if (exp_typ == RES_FORWARD) return "forward";
    else if (exp_typ == RES_FUNCTION) return "function";
    else if (exp_typ == RES_IF) return "if";
    else if (exp_typ == RES_INTEGER) return "integer";
    else if (exp_typ == RES_READLN) return "readln";
    else if (exp_typ == RES_REAL) return "real";
    else if (exp_typ == RES_SORT) return "sort";
    else if (exp_typ == RES_STRING) return "string";
    else if (exp_typ == RES_THEN) return "then";
    else if (exp_typ == RES_TRUE) return "true";
    else if (exp_typ == RES_VAR) return "var";
    else if (exp_typ == RES_WHILE) return "while";
    else if (exp_typ == RES_WRITE) return "write"; 
    else if (exp_typ == RES_REPEAT) return "repeat";
    else if (exp_typ == RES_UNTIL) return "until"; 

	else if (exp_typ == GENERAL) return "identifier";
	else if (exp_typ == INTEGER) return "integer";
	else if (exp_typ == REAL) return "real";
	else if (exp_typ == END) return "eof";
	else if (exp_typ == STRING) return "string";
	//else if (exp_typ == OTHER) return "undefined symbol";
	
	else if (exp_typ == OPERATOR_ASSIGN) return ":=";
	else if (exp_typ == OPERATOR_COLON) return ":";
	else if (exp_typ == OPERATOR_NOT_EQUAL) return "<>";
	else if (exp_typ == OPERATOR_LESS) return "<";
	else if (exp_typ == OPERATOR_LESS_EQUAL) return "<=";
	else if (exp_typ == OPERATOR_GREATER) return ">";
	else if (exp_typ == OPERATOR_GREATER_EQUAL) return ">=";
	else if (exp_typ == OPERATOR_EQUAL) return "=";
	else if (exp_typ == OPERATOR_SEMICOLON) return ";";
	else if (exp_typ == OPERATOR_MUL) return "*";
	else if (exp_typ == OPERATOR_DIV) return "/";
	else if (exp_typ == OPERATOR_PLUS) return "+";
	else if (exp_typ == OPERATOR_MINUS) return "-";
	else if (exp_typ == OPERATOR_BRACKET_OPEN) return "(";
	else if (exp_typ == OPERATOR_BRACKET_CLOSE) return ")";
	else if (exp_typ == OPERATOR_CURLY_BRACKET_OPEN) return "{";
	else if (exp_typ == OPERATOR_CURLY_BRACKET_CLOSE) return "}";
	else if (exp_typ == OPERATOR_COMM) return ".";
	
    return NULL;
}
    











