 // scanner.c
 // Implementacia interpretu imperativneho jazyka IFJ14, 20.10.2014
 // Autor: Viktor Toth,xtothv01, FIT
 //        Peter Nagy,xnagyp01, FIT
 //        Dusan Zeliar,xzelia00, FIT
 //        Filip Mutnansky,xmutna00 , FIT
 //        Marek Ochodek,xochod01, FIT
 // Prelozene: gcc 4.8.2
 // .c subor scanner.c
 // lexikalny analyzator

#include "scanner.h"
#include "error.h"

typedef enum {
			S_START,            //startovaci stav
			S_NUMBER, 				  //nacitanie cisla
			S_STRING_ID, 			  //nacitanie retazca
			S_STRING_ESC,			  //ESC sekvencia v retazci
			S_STRING_END, 		  //koniec retazca
			S_GENERAL,				  //identifikator/rezervovane slovo
			S_COLON, 				    //nacitanie dvojbodky
			S_OPERATOR_LESS, 		//operator mensi ako
			S_OPERATOR_GREATER, //operator vacsi ako
			S_NUMBER_DOT, 			//desatinne cislo
			S_NUMBER_E,				  //cislo v exponencialnom tvare
			S_NUMBER_REAL,			//realne cislo (desatinne)
			S_COMMENT				    //komentar - zahadzuje sa, neposiela sa do syntaktickej analyzi
	}tFSMState;


tToken tokenQueue;					  //umoznuje pozriet sa o token dopredu
bool queueFull = false;				//indikuje naplneny queue
char loadedCharBefore = 0;		//znak pred poslednym nacitanym znakom - kontrola konca suboru




void arrClear(char *chArr, int* pocetPrvkov){
	for (int j = 0; j <= *pocetPrvkov; ++j) {
		chArr[j] = 0;
	}
	free(chArr); //uvolnenie pamete pouzivane na ukladanie nacitanych lexem
}


/* case unsensitive */
void toLowerCase(char *chArr, int* size_chArr){
	for(int i = 0; i<*size_chArr; i++){
  		chArr[i] = tolower(chArr[i]);
	}

}


/* nacitanie dalsieho tokenu - token vracia cez datovy typ tToken */
bool getNextToken (FILE* file, tToken * token){

	if (queueFull == true) {	// odovzdanie tokenu pripraveneho v queue
		*token = tokenQueue;
		queueFull = false;
		return true;
	}

	char * chArr = NULL;       // char array
	int i = 0;                 // index - char array
	int size_chArr = 5;
	char stringESCSequence[3]; // ulozenie esc sekvencii - max. hodnota 255
	int stringESCIndex = 0;    // index do pola escape sekvencii
	int stringESCNum;	         // cislo esc sekvencie

	//Create buffer
	if( (chArr = malloc(size_chArr * sizeof(char))) == NULL) { //alokacia pamate pre pole znakov
        InternalError("Internal error - Nie je k dispozicii dostatok pamate\n");
        return false;
    }
	

	tFSMState state = S_START;     //FSM actual state

	bool endLex = false;           //ukoncenie cyklu

	char loadedChar = 0;           //posledny nacitany znak

	while( endLex == false && ((loadedChar = fgetc(file)) != EOF )) {

		if (i >= size_chArr){ //nedostatok miesta - alokacia dalsieho priestoru

			size_chArr = size_chArr*2;
			if((chArr = realloc(chArr, size_chArr*sizeof(char))) == NULL) {
        		InternalError("Internal error - Nie je k dispozicii dostatok pamate\n");
        		arrClear(chArr,&i); // clear and free chArr
        		return false;
    		}
		}


		switch(state){

			case S_START:
				if(isspace(loadedChar)){ //preskocenie bielych znakov
					state = S_START;
				}
				else if(isdigit(loadedChar)){ //cislo
					state = S_NUMBER;
					chArr[i] = loadedChar;
					i++;
				}
				else if(isalpha(loadedChar) || loadedChar == '_') { //identifikator moze zacinat znakom '_'
					state = S_GENERAL;
					chArr[i] = loadedChar;
					i++;
					token -> typ = GENERAL;
				}
				else if(loadedChar == '*') {  //aritmeticke operatory
					chArr[i] = loadedChar;
					i++;
					token -> typ = OPERATOR_MUL;
					loadedChar = fgetc(file); // nacitanie dalsieho znaku
					endLex = true;
				}
				else if(loadedChar == '+') {
					chArr[i] = loadedChar;
					i++;
					token -> typ = OPERATOR_PLUS;
					loadedChar = fgetc(file); // nacitanie dalsieho znaku
					endLex = true;
				}
				else if(loadedChar == '-') {
					chArr[i] = loadedChar;
					i++;
					token -> typ = OPERATOR_MINUS;
					loadedChar = fgetc(file); // nacitanie dalsieho znaku
					endLex = true; 
				}
				else if(loadedChar == '/') {
					chArr[i] = loadedChar;
					i++;
					token -> typ = OPERATOR_DIV;
					loadedChar = fgetc(file); // nacitanie dalsieho znaku
					endLex = true; 
				}
				else if(loadedChar == ':'){
					state = S_COLON;
					chArr[i] = loadedChar;
					i++;
				}
				else if (loadedChar == '<') { //smaller not equal
					state = S_OPERATOR_LESS;
					chArr[i] = loadedChar;
					i++;
				}
				else if(loadedChar == '>') { //operator greater
					state = S_OPERATOR_GREATER;
					chArr[i] = loadedChar;
					i++;
				}
				else if(loadedChar == '=') {
					chArr[i] = loadedChar;
					i++;
					token -> typ = OPERATOR_EQUAL;
					loadedChar = fgetc(file); // nacitanie dalsieho znaku
					endLex = true; //zastavenie
				}
				else if (loadedChar == '\'') {
					state = S_STRING_ID;
				}
				else if (loadedChar == '{') { //other operators
					state = S_COMMENT;
					i++;
				}
				else if(loadedChar == ','){
					chArr[i] = loadedChar;
					i++;
					token -> typ = OPERATOR_COMM;
					loadedChar = fgetc(file);
					endLex = true;
				}
				else if(loadedChar == ';'){
					chArr[i] = loadedChar;
					i++;
					token -> typ = OPERATOR_SEMICOLON;
					loadedChar = fgetc(file);
					endLex = true;
				}
				else if(loadedChar == '('){
					chArr[i] = loadedChar;
					i++;
					token -> typ = OPERATOR_BRACKET_OPEN;
					loadedChar = fgetc(file);
					endLex = true;
				}
				else if(loadedChar == ')'){
					chArr[i] = loadedChar;
					i++;
					token -> typ = OPERATOR_BRACKET_CLOSE;
					loadedChar = fgetc(file);
					endLex = true;
				}
				else if(loadedChar == '.'){
					chArr[i] = loadedChar;
					i++;
					token -> typ = OPERATOR_DOT;
					loadedChar = fgetc(file);
					endLex = true;
				}
				else {
					ScannerError("Chybna struktura aktualniho lexemu\n");
					arrClear(chArr,&i); // clear and free chArr
					return false;
				}
				break;

			case S_GENERAL:
				if (isalpha(loadedChar) || loadedChar == '_' || isdigit(loadedChar)) {
					state = S_GENERAL;
					chArr[i] = loadedChar;
					i++;
				}
				else {
					endLex = true;
				}
				break;

			case S_NUMBER:
				if (isdigit(loadedChar)) {
					state = S_NUMBER;
					chArr[i] = loadedChar;
					i++;
				}
				else if (loadedChar == '.') { //DOT
					state = S_NUMBER_DOT;
					chArr[i] = loadedChar;
					i++;
				}
				else if (loadedChar == 'e' || loadedChar == 'E') {
					state = S_NUMBER_E;
					chArr[i] = loadedChar;
					i++;
				}
				else{
						token -> typ = INTEGER;
						token -> attribute.integer = atoi(chArr); //odovzdanie atributu
						endLex = true;				
				}
				break;

			case S_NUMBER_DOT:
				if (isdigit(loadedChar)) {
					state = S_NUMBER_REAL;
					chArr[i] = loadedChar;
					i++;
				}
				else {
					ScannerError("Chybna struktura aktualniho lexemu\n");
					arrClear(chArr,&i); // clear and free chArr
					return false;
				}
				break;

			case S_NUMBER_E:
				if (isdigit(loadedChar) || ((loadedChar == '+' || loadedChar == '-') && (chArr[i-1] == 'e' || chArr[i-1] == 'E'))  ) {
					state = S_NUMBER_E;
					chArr[i] = loadedChar;
					i++;
				}
				else if (isdigit(chArr[i-1]) ) {
					token -> typ = REAL;
					token -> attribute.real = atof(chArr); //predavanie atributu cez datovy typ tToken
					endLex = true;
				}
				else {
					ScannerError("Chybna struktura aktualniho lexemu\n");
					arrClear(chArr,&i); // clear and free chArr
					return false;
				}
				break;

			case S_NUMBER_REAL:
				if (isdigit(loadedChar)) {
					state = S_NUMBER_REAL;
					chArr[i] = loadedChar;
					i++;
				}
				else if (loadedChar == 'e' || loadedChar == 'E') {
					state = S_NUMBER_E;
					chArr[i] = loadedChar;
					i++;
				}
				else {
						token -> typ = REAL;
						token -> attribute.real = atof(chArr); //predavanie atributu cez datovy typ tToken
						endLex = true;
				}

				break;

			case S_COLON:
				if (loadedChar == '=') {
					token -> typ = OPERATOR_ASSIGN;
					endLex = true;
					loadedChar = fgetc(file);
					i++;
				}
				else {
					token -> typ = OPERATOR_COLON;
					endLex = true;
				}
				break;

			case S_STRING_ID:
				if (loadedChar == '\'') {
					state = S_STRING_END; 
				}
				else if ( loadedChar > 31 ) {	// priamy zapis iba znakov s ASCII > 31
					state = S_STRING_ID;
					chArr[i] = loadedChar;
					i++;
				}
				else {
					ScannerError("Chybna struktura aktualniho lexemu\n");
					arrClear(chArr,&i); // clear and free chArr
					return false;
				}
				break;

			case S_STRING_ESC:
				if (isdigit(loadedChar)) {
					state = S_STRING_ESC;

					if (stringESCIndex == 3) {	// vzniklo by stvorcifernce cislo - obmedzenie 255
						ScannerError("Chybna struktura aktualniho lexemu\n");
						arrClear(chArr,&i); // clear and free chArr
						return false;
					}

					stringESCSequence[stringESCIndex] = loadedChar;
					stringESCIndex++;

					if ( stringESCSequence[0] == '0') { // odstranenie prebytocnych nul na zaciatku
						stringESCIndex--;
					}
					
				}
				else if ( !isdigit(stringESCSequence[stringESCIndex-1]) ) { //predosli znak nie je cislo
					ScannerError("Chybna struktura aktualniho lexemu\n");
					arrClear(chArr,&i); // clear and free chArr
					return false;
				}
				else if (loadedChar == '\'') {
					state = S_STRING_ID;
					stringESCNum = atoi(stringESCSequence);

					if (stringESCNum < 1 || stringESCNum > 255) {
						ScannerError("Chybna struktura aktualniho lexemu\n");
						arrClear(chArr,&i); // clear and free chArr
						return false;
					}
					chArr[i] = atoi(stringESCSequence);
					i++;
					stringESCIndex = 0;	// nulovanie indexu do pola esc sekvencii
				}
				else {
					ScannerError("Chybna struktura aktualniho lexemu\n");
					arrClear(chArr,&i); // clear and free chArr
					return false;
				}
				break;

			case S_STRING_END:
				if (loadedChar == '#') {
					state = S_STRING_ESC;
				}
				else if (loadedChar == '\'') {	// dva apostrofy za sebou - znak apostrof
					state = S_STRING_ID;
					chArr[i] = loadedChar;
					i++;
				}
				else {
					token -> typ = STRING;
					if ((token -> attribute.string = malloc((i+1)*sizeof(char))) == NULL){ //alokacia priestoru pre string v token - POTREBNE UVOLNIT!
						InternalError("Internal error - Nie je k dispozicii dostatok pamate\n");
						arrClear(chArr,&i); // clear and free chArr
						return false;
					}

					strncpy(token -> attribute.string, chArr, i); //kopirovanie stringu do attribute
					token->attribute.string[i]='\0';
					endLex = true;
				}
				break;

			case S_OPERATOR_LESS:
				if (loadedChar == '>') {
					chArr[i] = loadedChar;
					i++;
					token -> typ = OPERATOR_NOT_EQUAL;
					loadedChar = fgetc(file); // nacitanie dalsieho znaku
					endLex = true; //zastavenie
				}
				else if (loadedChar == '=') { //mensie alebo rovne
					chArr[i] = loadedChar;
					i++;
					token -> typ = OPERATOR_LESS_EQUAL;
					loadedChar = fgetc(file); // nacitanie dalsieho znaku
					endLex = true;
				}
				else {
					chArr[i] = loadedChar;
					i++;
					token -> typ = OPERATOR_LESS;
					endLex = true;
				}
				break;

			case S_OPERATOR_GREATER:
				if (loadedChar == '=') {
					chArr[i] = loadedChar;
					i++;
					token -> typ = OPERATOR_GREATER_EQUAL;
					loadedChar = fgetc(file); // nacitanie dalsieho znaku
					endLex = true;
				}
				else {
					chArr[i] = loadedChar;
					i++;
					token -> typ = OPERATOR_GREATER;
					endLex = true;
				}
				break;

			case S_COMMENT:
				if (loadedChar == '}') {
					state = S_START;
					i = 0;	// vynulovanie indexu do pola
				}
				else {
					i++;
				}
				break;

			default:
				ScannerError("Chybna struktura aktualniho lexemu\n"); // chyba lexikalnej analyzi - dana lexema je nedefinovana
				arrClear(chArr,&i); // clear and free chArr
				return false;
		}

	}

	if (state == S_STRING_ID) { // neukoncenie stringu pred koncom suboru
		ScannerError("Chybna struktura aktualniho lexemu\n");
		arrClear(chArr,&i); // clear and free chArr 
		return false;
	}

  if (state == S_COMMENT) { // neukonceny komentar pred koncom suboru
    ScannerError("Chybna struktura aktualniho lexemu\n");
    arrClear(chArr,&i); // clear and free chArr 
    return false;
  }

	// kontrola konca suboru
	if (((loadedChar == EOF) && (loadedCharBefore == EOF || isspace(loadedCharBefore))) ){
		token -> typ = END;
	}
	else { // vratenie znaku naspat do streamu
		ungetc(loadedChar, file);
	}

	loadedCharBefore = loadedChar; // predposledny nacitany znak

	//porovnanie s klucovymi slovami jazyka
	if (token -> typ == GENERAL){

		toLowerCase(chArr, &size_chArr); //case insensitive

		if ((token -> attribute.string = malloc((i+1)*sizeof(char))) == NULL){ //alokacia priestoru pre string v token - POTREBNE UVOLNIT!	
			InternalError("Internal error - Nie je k dispozicii dostatok pamate\n");
			arrClear(chArr,&i); // clear and free chArr
			return false;
		}

		strncpy(token -> attribute.string, chArr, i); //kopirovanie stringu do attribute
		token->attribute.string[i]='\0';


		if (strcmp(token->attribute.string, "begin") == 0 )
			token -> typ = RES_BEGIN;
		else if (strcmp(token->attribute.string, "boolean") == 0 )
			token -> typ = RES_BOOL;
		else if (strcmp(token->attribute.string, "do") == 0 )
			token -> typ = RES_DO;
		else if (strcmp(token->attribute.string, "else") == 0 )
			token -> typ = RES_ELSE;
		else if (strcmp(token->attribute.string, "end") == 0 )
			token -> typ = RES_END;
		else if (strcmp(token->attribute.string, "false") == 0 )
			token -> typ = RES_FALSE;
		else if (strcmp(token->attribute.string, "find") == 0 )
			token -> typ = RES_FIND;
		else if (strcmp(token->attribute.string, "forward") == 0 )
			token -> typ = RES_FORWARD;
		else if (strcmp(token->attribute.string, "function") == 0 )
			token -> typ = RES_FUNCTION;
		else if (strcmp(token->attribute.string, "if") == 0 )
			token -> typ = RES_IF;
		else if (strcmp(token->attribute.string, "integer") == 0 )
			token -> typ = RES_INTEGER;
		else if (strcmp(token->attribute.string, "readln") == 0 )
			token -> typ = RES_READLN;
		else if (strcmp(token->attribute.string, "real") == 0 )
			token -> typ = RES_REAL;
		else if (strcmp(token->attribute.string, "sort") == 0 )
			token -> typ = RES_SORT;
		else if (strcmp(token->attribute.string, "string") == 0 )
			token -> typ = RES_STRING;
		else if (strcmp(token->attribute.string, "then") == 0 )
			token -> typ = RES_THEN;
		else if (strcmp(token->attribute.string, "true") == 0 )
			token -> typ = RES_TRUE;
		else if (strcmp(token->attribute.string, "var") == 0 )
			token -> typ = RES_VAR;
		else if (strcmp(token->attribute.string, "while") == 0 )
			token -> typ = RES_WHILE;
		else if (strcmp(token->attribute.string, "write") == 0 )
			token -> typ = RES_WRITE;
		else if (strcmp(token->attribute.string, "repeat") == 0 )
			token -> typ = RES_REPEAT;
		else if (strcmp(token->attribute.string, "until") == 0 )
			token -> typ = RES_UNTIL;



	}

	arrClear(chArr,&i); // clear and free chArr

	return true;

}

bool lookNextToken(FILE* file, tToken * token){
	getNextToken (file, &tokenQueue);

	*token = tokenQueue;	// odovzdanie tokenu
	queueFull = true;		// indikacia naplneneho queue
	
	return true;
}

