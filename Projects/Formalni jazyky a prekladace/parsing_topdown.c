 // parsing_topdown.c
 // Implementacia interpretu imperativneho jazyka IFJ14, 15.10.2014
 // Autor: Viktor Toth,xtothv01, FIT
 //        Peter Nagy,xnagyp01, FIT
 //        Dusan Zeliar,xzelia00, FIT
 //        Filip Mutnansky,xmutna00 , FIT
 //        Marek Ochodek,xochod01, FIT
 // Prelozene: gcc 4.8.2
 // .c subor parsing_topdown.c
 // syntakticka analyza zhora dole 

//======================== FILES INCLUDED ===============================
#include "scanner.h"
#include "parsing.h"
#include <stdbool.h>
#include "error.h"
#include "ial.h"
#include "sem.h"
#include "interpret.h"

//======================== FUNCTIONS ===============================

tToken act_token;
TStack_tab *s = NULL;
char *parTypes = NULL;
tListOfInstr *list;
char expType;
int parIndex = 0;
int parIndex2 = 0;
char *params;
Thtab_t *globTable;
bool inFunction;
char *actFunction;
char *callingFunc;

/*<Program>*/
bool prog() {
	
	if ((getNextToken ( source_file, &act_token)) == false) return false;
	
	/*--------------------------------*/
	globTable = initTable(HSIZE);
	importFunctions();
	inFunction = false;				// nie sme vo funkcii
	/*--------------------------------*/
	/*<Definicia_prem>*/
	if ((first(1,RES_VAR))){
		if ( !dpre()) return false;
	}
	else if (!first(2,RES_FUNCTION,RES_BEGIN)) return parserError(2,RES_FUNCTION,RES_BEGIN);
	/*<Definicia_fun>*/
	if (first(1,RES_FUNCTION)) {
		if ( !dfun()) return false;
	}
	else if (!first(1,RES_BEGIN)) return parserError(1,RES_BEGIN);	
	/*<Main_blok>*/
	if (first(1,RES_BEGIN)) {
		if ( !mblo()) return false;
	}
	else {
		return parserError(3,RES_VAR,RES_FUNCTION,RES_BEGIN);
	}
	ErrorPrint("OK \n");
	return true;		
}

/*<Definicia_prem>*/
bool dpre() {
    /*var*/
    if (!expect(RES_VAR)) return false;
    /*<Premenna>*/
    if (first(1,GENERAL)) {
		if ( !prem()) return false;
	}
	else return parserError(1,GENERAL);
return true;
}

/*<Premenna>*/
bool prem() {
	/*id*/
	/*----------------------------*/
	char *id = act_token.attribute.string;
	/*----------------------------*/
    if (!expect(GENERAL)) return false;
    
    /*:*/
    if (!expect(OPERATOR_COLON)) return false;
    /*<Typ>*/
    /*----------------------------*/
    char type = act_token.attribute.string[0];
    /*----------------------------*/
    if (first(4,RES_BOOL,RES_INTEGER,RES_REAL,RES_STRING)) {
		if ( !typ()) return false;
	}
	else return parserError(4,RES_BOOL,RES_INTEGER,RES_REAL,RES_STRING);
	/*;*/
    if (!expect(OPERATOR_SEMICOLON)) return false;
    
    /*----------------------------*/
    tSymbol *sym;
   
    if (inFunction)
    {
		// Hladanie premennej v lokalnej tabulke
		if (h_Lookup(h_Lookup(globTable,actFunction)->funcTab,id) != NULL || findParamSem(id) != NULL || !strcmp(id,actFunction) )
			return SemError();
	} else
		{
			// Hladanie premennej v globalnej tabulke
			if (h_Lookup(globTable,id) != NULL)
				return SemError();
		}
				
    if (inFunction)
		sym = h_Insert(h_Lookup(globTable,actFunction)->funcTab,id); // vlozime do lokalnej tabulky
		else
			sym = h_Insert(globTable,id);			// vlozime do globalnej tabulky
    
    if (sym == NULL)
    {
		InternalError("Allocation error\n");
		return false;
	}
	
	// nastavime typ, inicializaciu
    sym->type = type;			
    sym->state = false;
    sym->funcOrVar = true;
    /*----------------------------*/
    /*<E>*/
    if (first(1,GENERAL)) {
		if ( !e()) return false;
	}
	else if (!first(2,RES_FUNCTION,RES_BEGIN)) return parserError(2,RES_FUNCTION,RES_BEGIN);
return true;
}

/*<Typ>*/
bool typ() {
	if (first(1,RES_BOOL)){
		if (!expect(RES_BOOL)) return false;
	}
	else if (first(1,RES_INTEGER)){
		if (!expect(RES_INTEGER)) return false;
	}
	else if (first(1,RES_REAL)){
		if (!expect(RES_REAL)) return false;
	}
	else if (first(1,RES_STRING)){
		if (!expect(RES_STRING)) return false;
	}
	else return parserError(4,RES_BOOL,RES_INTEGER,RES_REAL,RES_STRING); 

	return true;
}

/*<E>*/
bool e() {
	/*<Premenna>*/
    if ( !prem()) return false;

	return true;
}

/*<Definicia_fun>*/
bool dfun(){
   	/*<Hlavicka*/        	
    if (first(1,RES_FUNCTION)){
        if ( !hlav()) return false;
    }
    else return parserError(1,RES_FUNCTION);
    /*<Q>*/
    if (first(3,RES_BEGIN,RES_FORWARD,RES_VAR)){
        if ( !q()) return false;
    }
    else return parserError(3,RES_BEGIN,RES_FORWARD,RES_VAR);

	return true;
}

/*<Hlavicka>*/
bool hlav (){
	/*function*/
	if (!expect(RES_FUNCTION)) return false;
	
	/*id*/
	char *id = act_token.attribute.string;
	/*----------------------------------------*/
	tSymbol *sym;
	
	sym = h_Lookup(globTable,id);
	
	if ((sym != NULL) && (sym->defined == true))		// ak uz raz bola definovana
		return SemError();
		else
			if ((sym != NULL) && (sym->defined == false))  // ak je predosla forward deklaracia
				sym->defined = true;
				
	if (sym == NULL)
	{
		if ((sym = h_Insert(globTable,id)) == NULL)		// vlozime do tabulky
		{	
			InternalError("Allocation error\n");
			return false;
		}	
		sym->defined = false;
	}
	
	sym->funcTab=initTable(HSIZE);							// inicializacia lokalnej tabulky
	inFunction = true;
	actFunction = id;
	sym->funcOrVar = false;
	sym->instructions = malloc(sizeof(tListOfInstrFun));
	listInitFun(sym->instructions);							// inicializacia instrukcii funkcie
	/*----------------------------*/
	if (!expect(GENERAL)) return false;
	
	/*(*/
	if (!expect(OPERATOR_BRACKET_OPEN)) return false;
	/*<Zoznam_param>*/
	if (first(1,GENERAL)){
		if (!zpar()) return false;
	}
	/*)*/
	if (!expect(OPERATOR_BRACKET_CLOSE)) return false;
	/*:*/
	if (!expect(OPERATOR_COLON)) return false;
	/*<Typ>*/
	/*----------------------------*/
	char type = act_token.attribute.string[0];
	/*----------------------------*/
    if (first(4,RES_BOOL,RES_INTEGER,RES_REAL,RES_STRING)) {
		if ( !typ()) return false;
	}
	else return parserError(4,RES_BOOL,RES_INTEGER,RES_REAL,RES_STRING);
	
	/*;*/
	if (!expect(OPERATOR_SEMICOLON)) return false;
	/*----------------------------*/
	// kontrola parametrov funkcie
	if (sym->funcParams)								
		if ((strlen(sym->funcParams) != (unsigned)parIndex2) || (type != sym->type))
			return SemError();
	
	parIndex2 = 0;		
    sym->type = type;
    sym->funcOrVar = false;
    sym->funcParams = parTypes;
    parTypes = NULL;
	/*----------------------------*/
	return true;
}

/*<Q>*/
bool q () {
	if (first(2,RES_VAR,RES_BEGIN)) {
  	 	/*<Definicia_prem>*/
 	  	if (first(1,RES_VAR)){
  	 		if (!dpre()) return false;
 	  	} 
 	  	else if (!first(1,RES_BEGIN)) return parserError(1,RES_BEGIN);	
 	  	/*<F_Blok>*/
		if (first(1,RES_BEGIN)) {
			if ( !fblo()) return false;
		}
		else return parserError(1,RES_BEGIN);
		/*---------------------------------------------*/
		//PUSH RETURN VALUE
		texpVal *tmp = malloc(sizeof(texpVal));
		tmp->typ = h_Lookup(globTable,actFunction)->type;
		tmp->varOrCon = false;
		tmp->id = actFunction;
		tmp->val = h_Lookup(globTable,actFunction)->value;
		
		generateInstructionFun(h_Lookup(globTable,actFunction)->instructions,I_PUSHVAL ,tmp , NULL , NULL);
		
		inFunction = false;
		h_Lookup(globTable,actFunction)->defined = true;	// je definovana
		/*---------------------------------------------*/
		/*<Definica_fun*/
 		if (first(1,RES_FUNCTION)) {
 			if ( !dfun()) return false;
 		}
 		else if (!first(1,RES_BEGIN)) return parserError(1,RES_BEGIN);	
	}
	else if(first(1,RES_FORWARD)) {
		/*forward*/
		if (!expect(RES_FORWARD)) return false;
		/*;*/
	    if (!expect(OPERATOR_SEMICOLON)) return false;
	    /*----------------------------------------------------*/
	    inFunction = false;
	    
	    tSymbol *tmp = h_Lookup(globTable,actFunction);
	    
	    if (tmp->defined == true)		// ak je forward uz za definovanou funkciou
			return SemError();		
	    /*----------------------------------------------------*/
	    /*<Definica_fun*/
 		if (first(1,RES_FUNCTION)) {
 			if ( !dfun()) return false;
 		}
 		else if (!first(1,RES_BEGIN)) return parserError(1,RES_BEGIN);	
	}
	return true;
}

/*<Zoznam_param>*/
bool zpar () {
	if (first(1,GENERAL)) {
		/*id*/
		char *id = act_token.attribute.string;
		if (!expect(GENERAL)) return false;
		/* : */
		if (!expect(OPERATOR_COLON)) return false;
		/*<Typ>*/
        /*---------------------------------------*/
        char type = act_token.attribute.string[0];
        /*---------------------------------------*/
        if (first(4,RES_BOOL,RES_INTEGER,RES_REAL,RES_STRING)) {
		    if ( !typ()) return false;
	    }
   	    else return parserError(4,RES_BOOL,RES_INTEGER,RES_REAL,RES_STRING);
   	    /*---------------------------------------*/
   	    
   	    if (!strcmp(id,actFunction))
			return SemError();
   	       	    
		if ((parTypes = addCharToString(parTypes,type)) == NULL)
		{
			InternalError("Allocation error\n");
			return false;
		}
		
		// PUSHPARAM
		tSymbol *fun = h_Lookup(globTable,actFunction);
				
		if (fun->defined && !(fun->funcParams))			// ak nesedia pocty parametrov
			return SemError();
		
		if (fun->defined == true && fun->funcParams)
		{
			if ((unsigned)parIndex2 < strlen(fun->funcParams))	
			{   // ak nesedia typy a id parametrov vo forward a teraz
				if (strcmp(fun->params[parIndex2]->key,id) || (type != fun->params[parIndex2]->type))	
					return SemError();
			} else
				return SemError();
		} else
			{
				if ((fun->params[parIndex2] = malloc((sizeof(tSymbol) + strlen(id) + 1))) == NULL) // alokujeme miesto pre parameter
					return NULL;
		
				strncpy(fun->params[parIndex2]->key, id, strlen(id));
				fun->params[parIndex2]->type = type;
						
			}
		parIndex2++;    // posun na dalsi parameter
	    /*---------------------------------------*/ 
   	    /*F*/
   	    if (first(1,OPERATOR_SEMICOLON)) {
   	    	if (!f()) return false;
   	    }
	}
	return true;
}

/*<F>*/
bool f () {
	if (first(1,OPERATOR_SEMICOLON)) {
		/* ; */
		if (!expect(OPERATOR_SEMICOLON)) return false;
		/*<Zoznam_param>*/
		if (first(1,GENERAL)){
	    	if (!zpar()) return false;
	    } 
	    else return parserError(1,GENERAL);
	}
	return true;
}

/*<F_Blok>*/
bool fblo () {
	/*<Blok>*/
	if (first(1,RES_BEGIN)) {
		if ( !blok()) return false;
	}
	else return parserError(1,RES_BEGIN);
	/* ; */
	if (!expect(OPERATOR_SEMICOLON)) return false;
	return true;
}

bool blok () {
	/* begin */
	if (!expect(RES_BEGIN)) return false;
	/* <Zoznam_prik> */
	if (first(7,RES_BEGIN,GENERAL,RES_IF,RES_WHILE,RES_REPEAT,RES_READLN,RES_WRITE)) {
		if ( !zpri()) return false;
	}
	/* end */
	if (!expect(RES_END)) return false;
	return true;
}

/*<Zoznam_prik>*/
bool zpri () {
	/*<Prikaz>*/
	if (first(7,RES_BEGIN,GENERAL,RES_IF,RES_WHILE,RES_REPEAT,RES_READLN,RES_WRITE)) {
		if ( !prik()) return false;
	}
	/*<G>*/
	if (first(1,OPERATOR_SEMICOLON)) {
		if ( !g()) return false;
	} 
	return true;
}

/*G*/
bool g () {
	/* ; */
	if (!expect(OPERATOR_SEMICOLON)) return false;
	/*<Prikaz>*/
	if (first(7,RES_BEGIN,GENERAL,RES_IF,RES_WHILE,RES_REPEAT,RES_READLN,RES_WRITE)) {
		if ( !prik()) return false;
	}
	else return parserError(7,RES_BEGIN,GENERAL,RES_IF,RES_WHILE,RES_REPEAT,RES_READLN,RES_WRITE);
	/*<G>*/
	if (first(1,OPERATOR_SEMICOLON)) {
		if ( !g()) return false;
	} 
	return true;
}

/*<Prikaz>*/
bool prik () {
	if (first(1,RES_BEGIN)) {
		/* <Blok> */
		if ( !blok()) return false;
	}
	else if (first(1,GENERAL)) {
		/*id*/
	   /*---------------------------------------*/
	   char *id = act_token.attribute.string;
 	   /*---------------------------------------*/
 	   if (!expect(GENERAL)) return false;
 	   /*---------------------------------------*/
 	   tSymbol *tmp;
 	   
 	   if (inFunction)
		tmp = findSymbolInFunSem(id);
		else
			tmp = h_Lookup(globTable,id);
 	   
 	   if (tmp == NULL || (inFunction == false && tmp->funcOrVar == false))	// kontrola deklaracie premennej
		   return SemError();
		   
 	   /*---------------------------------------*/
 	   /* := */
 	   if (!expect(OPERATOR_ASSIGN)) return false;
 	   /* <FunVyraz> */
 	   if ( !funvyraz()) return false;
 	   /*---------------------------------------*/
 	   if (tmp->type != expType)
			return SemError2();
			
	   // Priradenie hodnoty z vrcholu zasobniku
	   if (inFunction == false)	   
		   generateInstruction(list, I_ASSIGN, id, NULL, NULL);
		   else
			  generateInstructionFun(h_Lookup(globTable,actFunction)->instructions, I_ASSIGN, id, NULL, NULL);
 	   /*---------------------------------------*/
	}
	else if (first(1,RES_IF)) {
		/* if */
		if (!expect(RES_IF)) return false;
		/* <If_podmienka> */
		if ( !ifpo()) return false;
	}
	else if (first(1,RES_WHILE)) {	
		/* while */
		if (!expect(RES_WHILE)) return false;
    	/*-------------------------------------------------*/
	    numOfLabels++;
	    // navestie
	    if (inFunction == false)	   
	        generateInstruction(list,I_LAB,generateLabelName(),NULL,NULL);
	        else
				generateInstructionFun(h_Lookup(globTable,actFunction)->instructions,I_LAB,generateLabelName(),NULL,NULL);
	    /*-------------------------------------------------*/
		/* <Vyraz>  */
		if ( !vyraz()) return false;
		/* do */
		if (!expect(RES_DO)) return false;
		/*-------------------------------------------------*/
		if (expType != 'b')
			return SemError2();
		//skok, alebo pokracovanie
		numOfLabels++;
		if (inFunction == false)
			generateInstruction(list,I_GO_OR_STAY,generateLabelName(),NULL,NULL);
			else
				generateInstructionFun(h_Lookup(globTable,actFunction)->instructions,I_GO_OR_STAY,generateLabelName(),NULL,NULL);
    	/*-------------------------------------------------*/	
		/*<Blok>*/
    	if (first(1,RES_BEGIN)) {
        	if ( !blok()) return false;
	    }else return parserError(1,RES_BEGIN);
	    /*-------------------------------------------------*/
	    //navestie
	    if (inFunction == false)
			generateInstruction(list,I_LAB,generateLabelName(),NULL,NULL);
			else
				generateInstructionFun(h_Lookup(globTable,actFunction)->instructions,I_LAB,generateLabelName(),NULL,NULL);
	    
	    numOfLabels--;
		// skok, alebo pokracovanie
	    if (inFunction == false)
			generateInstruction(list,I_GO_BACK_OR_STAY,generateLabelName(),NULL,NULL);
			else
				generateInstructionFun(h_Lookup(globTable,actFunction)->instructions,I_GO_BACK_OR_STAY,generateLabelName(),NULL,NULL);
	    
	    numOfLabels--;
	    /*-------------------------------------------------*/
	}
	else if (first(1,RES_REPEAT)) {
		/* repeat */
		if (!expect(RES_REPEAT)) return false;
    	/*-------------------------------------------------*/
	    numOfLabels++;
	    // navestie
	    if (inFunction == false)	   
	        generateInstruction(list,I_LAB,generateLabelName(),NULL,NULL);
	        else
				generateInstructionFun(h_Lookup(globTable,actFunction)->instructions,I_LAB,generateLabelName(),NULL,NULL);
	    /*-------------------------------------------------*/
		/* <Zoznam_prik> */
    	if (first(7,RES_BEGIN,GENERAL,RES_IF,RES_WHILE,RES_REPEAT,RES_READLN,RES_WRITE)) {
    		if ( !zpri()) return false;
    	}else return parserError(7,RES_BEGIN,GENERAL,RES_IF,RES_WHILE,RES_REPEAT,RES_READLN,RES_WRITE);
    	/* end */
	    if (!expect(RES_UNTIL)) return false;
		/* <Vyraz>  */
		if ( !vyraz()) return false;
	    /*-------------------------------------------------*/
	    if (expType != 'b')
			return SemError2();
		// znegovanie hodnoty na zasobniku	
	    if (inFunction == false){
	    	generateInstruction(list,I_REVERSE_BOOL,NULL,NULL,NULL);
			generateInstruction(list,I_GO_BACK_OR_STAY,generateLabelName(),NULL,NULL);
			}else{
				generateInstructionFun(h_Lookup(globTable,actFunction)->instructions,I_REVERSE_BOOL,NULL,NULL,NULL);
				generateInstructionFun(h_Lookup(globTable,actFunction)->instructions,I_GO_BACK_OR_STAY,generateLabelName(),NULL,NULL);
			}
	    
	    numOfLabels--;
	    /*-------------------------------------------------*/
	}
	else if (first(1,RES_READLN)) {
		/* readln */
		if (!expect(RES_READLN)) return false;
		/* ( */
		if (!expect(OPERATOR_BRACKET_OPEN)) return false;
		/* id */
		/*---------------------------------------*/
		char *id = act_token.attribute.string;
		/*---------------------------------------*/
		
		if (!expect(GENERAL)) return false;
		/*---------------------------------------*/
		tSymbol *tmp;
		if (inFunction)
		tmp = findSymbolInFunSem(id);
		else
			tmp = h_Lookup(globTable,id);
		//semanticke kontroly
		if (tmp == NULL)
			return SemError();
			
		if (tmp->type == 'b') {
			CompatibilityError("Readln value can not be stored in boolean variable");
			return false;
		}
		/*---------------------------------------*/ 
		/*Vytvorenie instrukcie pre nacitanie*/
		if (inFunction == false)
			createReadlnInstruction(id);
			else
				createReadlnInstructionFun(id);
		/* ) */
		if (!expect(OPERATOR_BRACKET_CLOSE)) return false;
	}
	else if (first(1,RES_WRITE)) {
		/* write */
		if (!expect(RES_WRITE)) return false;
		/* ( */
		if (!expect(OPERATOR_BRACKET_OPEN)) return false;
		/* <Vypis> */
		if (first(6,GENERAL,INTEGER,REAL,STRING,RES_TRUE,RES_FALSE)) {
			if ( !vypi()) return false;
		}
		else return parserError(6,GENERAL,INTEGER,REAL,STRING,RES_TRUE,RES_FALSE);
	}

	return true;
}


/*<If_podmienka> */
bool ifpo() {
	/* <Vyraz> */
	if ( !vyraz()) return false;	
	/* then */
	if (!expect(RES_THEN)) return false;
	/*-------------------------------------------------*/
	if (expType != 'b')
		return SemError2();
	// skok, alebo pokracovanie
	numOfLabels++;
	if (inFunction == false)
		generateInstruction(list,I_GO_OR_STAY,generateLabelName(),NULL,NULL);
		else
			generateInstructionFun(h_Lookup(globTable,actFunction)->instructions,I_GO_OR_STAY,generateLabelName(),NULL,NULL);
	/*-------------------------------------------------*/	
	/*<Blok>*/
	if (first(1,RES_BEGIN)) {
		if ( !blok()) return false;
	}
	else return parserError(1,RES_BEGIN);
	/*--------------------------------------------------*/
	// navestie, skok , alebo pokracovanie
	if (inFunction == false)
	{
		generateInstruction(list,I_LAB,generateLabelName(),NULL,NULL);
		generateInstruction(list,I_GO_OR_STAY2,generateLabelName(),NULL,NULL);
	} else
		{
			generateInstructionFun(h_Lookup(globTable,actFunction)->instructions,I_LAB,generateLabelName(),NULL,NULL);
			generateInstructionFun(h_Lookup(globTable,actFunction)->instructions,I_GO_OR_STAY2,generateLabelName(),NULL,NULL);
		}
	/*--------------------------------------------------*/
	/* <H> */
	if (first(1,RES_ELSE)) {
		if ( !h()) return false;
	}
	// Zruesenie rozsirenia vetvy bez else-pokusne odvzdanie testuje len zakladnu verziu
	else return parserError(1,RES_ELSE);
	/*-------------------------------------------------------*/
	if (inFunction == false)
		generateInstruction(list,I_LAB,generateLabelName(),NULL,NULL);
		else
			generateInstructionFun(h_Lookup(globTable,actFunction)->instructions,I_LAB,generateLabelName(),NULL,NULL);
		
	numOfLabels--;
	/*-------------------------------------------------------*/
	return true;
}
/* <H> */
bool h() {
	/* else */
	if (!expect(RES_ELSE)) return false;
	
	/*<Blok>*/
	if (first(1,RES_BEGIN)) {
		if ( !blok()) return false;
	}
	return true;
}

/* <Vypis> */
bool vypi() {
		
	/*zisti typ tokenu pre generovanie
	  zavola funciu ktora generuje WRITE*/
	if (inFunction == false) 
	{
		if (!createWriteInstuction()) return false;
	} else
		{
			if (!createWriteInstuctionFun()) return false;
		}
			
	/* isTerminal */
	if (!isTerm()) return false;


	/* <J> */
	if (first(2,OPERATOR_COMM,OPERATOR_BRACKET_CLOSE)) {
		if ( !j()) return false;
	}
	else return parserError(2,OPERATOR_COMM,OPERATOR_BRACKET_CLOSE);
	return true;
}

/* <J> */
bool j () {
	if (first(1,OPERATOR_COMM)) {
		/* , */
		if (!expect(OPERATOR_COMM)) return false;
		/* <Vypis> */
		if (first(6,GENERAL,INTEGER,REAL,STRING,RES_TRUE,RES_FALSE)) {
			if ( !vypi()) return false;
		}
		else return parserError(6,GENERAL,INTEGER,REAL,STRING,RES_TRUE,RES_FALSE);
	}
	else if (first(1,OPERATOR_BRACKET_CLOSE)) {
		/* ) */
		if (!expect(OPERATOR_BRACKET_CLOSE)) return false;
	}
	return true;
}

/* <Vyraz> */
bool vyraz () {
	if (first(2,RES_DO,RES_THEN)){
        ParseExpressionError("No condition found\n");
        return false;
    }
	/* VOLANIE PRECEDENCNEJ ANALYZY */
	return parseExpression ();
}

bool mblo(){
	/*-----------------------------------*/
	//CHECK FORWARD DECLARATIONS
	if (checkForward() == false)	
		return SemError();	
	/*-----------------------------------*/
	/*<Blok>*/
	if (first(1,RES_BEGIN)) {
		if ( !blok()) return false;
	}
	else return parserError(1,RES_BEGIN);
	/* . OPERATOR_DOT */
	if (!expect(OPERATOR_DOT)) return false;
	/* END */
	if (!expect(END)) return false;

	return true;
}

/* id := Funkcia / vyraz */

bool funvyraz(){
	/*pomocna premenna pre ulozenie nasledujuceho tokenu */
    tToken tmp;
    if (!lookNextToken ( source_file, &tmp)) return false;	

	/*ak je prvy id a nasleduje ( , je to volanie funkcii */
	if (first(3,GENERAL,RES_SORT,RES_FIND) && tmp.typ==OPERATOR_BRACKET_OPEN){
		/*-----------------------------------------------------*/
		char *id = act_token.attribute.string;
		tSymbol *tmp2 = h_Lookup(globTable,id);
		
		// semanticke kontroly
 	    if (tmp2 == NULL || tmp2->funcOrVar == true)
		   return SemError();
				 
		params = tmp2->funcParams;
		callingFunc = id;		
		/*-----------------------------------------------------*/

		/* GENERAL */
		if (first(1,GENERAL)){
			if (!expect(GENERAL)) return false;
	    }else /* SORT */
		if (first(1,RES_SORT)){
			if (!expect(RES_SORT)) return false;
	    }else /* FIND */
		if (first(1,RES_FIND)){
			if (!expect(RES_FIND)) return false;
	    }
 	    /* ( */
	    if (!expect(OPERATOR_BRACKET_OPEN)) return false;
	    /*--------------------------------------------------------*/
	    /*Parameter */
	    if (first(6,GENERAL,INTEGER,REAL,STRING,RES_FALSE,RES_TRUE)) {
	    	if (!parameter()) return false;
	    }
	    /* ) */
	    if (!expect(OPERATOR_BRACKET_CLOSE)) return false;
	    /*-----------------------------------------------------*/    
	    expType=tmp2->type;
	    // typove kontroly
	    if ((strcmp(callingFunc,"length") && (strcmp(callingFunc,"copy") && (strcmp(callingFunc,"find") && (strcmp(callingFunc,"sort") ))) && params && ((unsigned)parIndex != strlen(params))))
			return SemError2();
							
	    parIndex = 0;
	    
	    //PUSH SYMBOL TABLE
	    if (inFunction == false)
			generateInstruction(list,I_PUSH_TABLE,tmp2->funcTab,h_Lookup(globTable,id)->params,NULL);
			else
				generateInstructionFun(h_Lookup(globTable,actFunction)->instructions,I_PUSH_TABLE,tmp2->funcTab,h_Lookup(globTable,id)->params,NULL);
	    
	    //CALL FUNCTION
	    if (!strcmp(callingFunc,"length"))
	    {
			if (inFunction == false)
				generateInstruction(list,I_LENGTH,NULL,NULL,NULL);
				else
					generateInstructionFun(h_Lookup(globTable,actFunction)->instructions,I_LENGTH,NULL,NULL,NULL);
		} else	if (!strcmp(callingFunc,"copy"))
			{
				if (inFunction == false)
					generateInstruction(list,I_COPY,NULL,NULL,NULL);
					else
						generateInstructionFun(h_Lookup(globTable,actFunction)->instructions,I_COPY,NULL,NULL,NULL);
			} else if (!strcmp(callingFunc,"find"))
				{
					if (inFunction == false)
						generateInstruction(list,I_FIND,NULL,NULL,NULL);
						else
							generateInstructionFun(h_Lookup(globTable,actFunction)->instructions,I_FIND,NULL,NULL,NULL);
				} else if (!strcmp(callingFunc,"sort"))
					{
						if (inFunction == false)
							generateInstruction(list,I_SORT,NULL,NULL,NULL);
							else
								generateInstructionFun(h_Lookup(globTable,actFunction)->instructions,I_SORT,NULL,NULL,NULL);
					} else 
						{	
							if (inFunction == false)
								generateInstruction(list,I_CALL_FUN,id,NULL,NULL);
								else
									generateInstructionFun(h_Lookup(globTable,actFunction)->instructions,I_CALL_FUN,id,NULL,NULL);
						}
		//POPTABLE
		if (inFunction == false)
			generateInstruction(list,I_POP_TABLE,NULL,NULL,NULL);
			else
				generateInstructionFun(h_Lookup(globTable,actFunction)->instructions,I_POP_TABLE,NULL,NULL,NULL);
		/*-----------------------------------------------------*/
	}
	/*inak je to vyraz*/
	else return vyraz();
	return true;
}

bool parameter(){
	/*--------------------------------------*/
	tToken tmp = act_token;
	/*--------------------------------------*/
	/* isTerminal */
	if (!isTerm()) return false;
	/*--------------------------------------*/	
	if (!(h_Lookup(globTable,callingFunc)->funcParams))
		return SemError2();	
		
	if (tmp.typ == GENERAL)
	{		
		tSymbol *tmp2;
		
		if (inFunction)
			tmp2 = findSymbolInFunSem(tmp.attribute.string);
			else
				tmp2 = h_Lookup(globTable,tmp.attribute.string);
				
		if (tmp2 == NULL)
			return SemError();
		
		// typova kontrola parametrov		
		if (tmp2->type != params[parIndex])
			return SemError2();
				
		if (inFunction == false)
		{
			if (tmp2->type == 'i')
				generateInstruction(list,I_PUSHPAR_INT,h_Lookup(globTable,callingFunc)->params[parIndex],tmp2->key,NULL);
				if (tmp2->type == 'r')
					generateInstruction(list,I_PUSHPAR_REAL,h_Lookup(globTable,callingFunc)->params[parIndex],tmp2->key,NULL);
					if (tmp2->type == 'b')
						generateInstruction(list,I_PUSHPAR_BOOL,h_Lookup(globTable,callingFunc)->params[parIndex],tmp2->key,NULL);
						if (tmp2->type == 's')							
							generateInstruction(list,I_PUSHPAR_STRING,h_Lookup(globTable,callingFunc)->params[parIndex],tmp2->key,NULL);
		}
			else
			{
				if (tmp2->type == 'i')
				generateInstructionFun(h_Lookup(globTable,actFunction)->instructions,I_PUSHPAR_INT,h_Lookup(globTable,callingFunc)->params[parIndex],tmp2->key,NULL);
				if (tmp2->type == 'r')
					generateInstructionFun(h_Lookup(globTable,actFunction)->instructions,I_PUSHPAR_REAL,h_Lookup(globTable,callingFunc)->params[parIndex],tmp2->key,NULL);
					if (tmp2->type == 'b')
						generateInstructionFun(h_Lookup(globTable,actFunction)->instructions,I_PUSHPAR_BOOL,h_Lookup(globTable,callingFunc)->params[parIndex],tmp2->key,NULL);
						if (tmp2->type == 's')
							generateInstructionFun(h_Lookup(globTable,actFunction)->instructions,I_PUSHPAR_STRING,h_Lookup(globTable,callingFunc)->params[parIndex],tmp2->key,NULL);
			}
			
	} else if (tmp.typ == INTEGER)
		{			
			if (params[parIndex] != 'i')
				return SemError2();
				
			int *val = malloc(sizeof(int));
			*val = tmp.attribute.integer;	
							
			if (inFunction == false)
				generateInstruction(list,I_PUSHPAR_INT,h_Lookup(globTable,callingFunc)->params[parIndex],NULL,val);
				else
					generateInstructionFun(h_Lookup(globTable,actFunction)->instructions,I_PUSHPAR_INT,h_Lookup(globTable,callingFunc)->params[parIndex],NULL, val);	
				
		} else if (tmp.typ == REAL)
			{
				if (params[parIndex] != 'r')
					return SemError2();
					
				double *val = malloc(sizeof(double));
				*val = tmp.attribute.real;	
							
			if (inFunction == false)
				generateInstruction(list,I_PUSHPAR_REAL,h_Lookup(globTable,callingFunc)->params[parIndex],NULL,val);
				else
					generateInstructionFun(h_Lookup(globTable,actFunction)->instructions,I_PUSHPAR_REAL,h_Lookup(globTable,callingFunc)->params[parIndex],NULL, val);	
				
					
			} else if (tmp.typ == STRING)
				{
					if (params[parIndex] != 's')
						return SemError2();
						
					char *val = malloc(strlen(tmp.attribute.string));
					val = tmp.attribute.string;	
										
					if (inFunction == false)
					{
						generateInstruction(list,I_PUSHPAR_STRING,h_Lookup(globTable,callingFunc)->params[parIndex],NULL,val);
					}
						else
							generateInstructionFun(h_Lookup(globTable,actFunction)->instructions,I_PUSHPAR_STRING,h_Lookup(globTable,callingFunc)->params[parIndex],NULL, val);	
					
					
				} else if (tmp.typ == RES_FALSE)
					{
						if (params[parIndex] != 'b')
							return SemError2();
							
						bool *val = malloc(sizeof(bool));
						*val = false;
										
						if (inFunction == false)
							generateInstruction(list,I_PUSHPAR_BOOL,h_Lookup(globTable,callingFunc)->params[parIndex],NULL,val);
							else
								generateInstructionFun(h_Lookup(globTable,actFunction)->instructions,I_PUSHPAR_BOOL,h_Lookup(globTable,callingFunc)->params[parIndex],NULL,val);		
						
					} else if (tmp.typ == RES_TRUE)
						{
							if (params[parIndex] != 'b')
								return SemError2();
															
							bool *val = malloc(sizeof(bool));
							*val = true;
										
							if (inFunction == false)
								generateInstruction(list,I_PUSHPAR_BOOL,h_Lookup(globTable,callingFunc)->params[parIndex],NULL,val);
								else
									generateInstructionFun(h_Lookup(globTable,actFunction)->instructions,I_PUSHPAR_BOOL,h_Lookup(globTable,callingFunc)->params[parIndex],NULL,val);		
						}
						
	parIndex++;
	/*--------------------------------------*/
	/* <K> */
	if (first(1,OPERATOR_COMM)) {
		if ( !k()) return false;
	}
	return true;
}

bool k() {
	/* , */
	if (!expect(OPERATOR_COMM)) return false;
	/*Parameter */
	if (first(6,GENERAL,INTEGER,REAL,STRING,RES_FALSE,RES_TRUE)) {
	   	if (!parameter()) return false;
	}

	return true;
}

bool createReadlnInstruction(char * id){

	tSymbol *tmp = h_Lookup(globTable,id);
	switch(tmp->type){
 		case 'b':
		  CompatibilityError("Readln value can not be stored in boolean variable");
		  return false;
		case 'i':
		  generateInstruction(list,I_READLN_INT ,&(tmp->value) , NULL , NULL);
		  break;
		case 'r':
		  generateInstruction(list,I_READLN_REAL ,&(tmp->value) , NULL , NULL);
		  break;
		case 's':
		  /*v pripade ze nieje inicializovana, nastavi  ukazatej na retazec na null*/
	      if (tmp->state == false) tmp->value.string=NULL;
		  generateInstruction(list,I_READLN_STRING ,&(tmp->value) , NULL , NULL);
		  break;
 	}
 	tmp->state=true;
	return true;
}

bool createReadlnInstructionFun(char * id){

	tSymbol *tmp = findSymbolInFunSem(id);
	switch(tmp->type){
 		case 'b':
		  CompatibilityError("Readln value can not be stored in boolean variable");
		  return false;
		case 'i':
		  generateInstructionFun(h_Lookup(globTable,actFunction)->instructions,I_READLN_INT ,&(tmp->value) , NULL , NULL);
		  break;
		case 'r':
		  generateInstructionFun(h_Lookup(globTable,actFunction)->instructions,I_READLN_REAL ,&(tmp->value) , NULL , NULL);
		  break;
		case 's':
		  /*v pripase ze nieje inicializovana, nastavi  ukazatej na retazec na null*/
	      if (tmp->state == false) tmp->value.string=NULL;
		  generateInstructionFun(h_Lookup(globTable,actFunction)->instructions,I_READLN_STRING ,&(tmp->value) , NULL , NULL);
		  break;
 	}
 	tmp->state=true;
	return true;
}

bool createWriteInstuction(){
	char tokenTyp;
	char * idTyp=&tokenTyp;
	tAttribute* pointerToData;
	bool* pointerToState = NULL;
    
    /* Pozrie sa ci bude vypisovat konstantu alebo identifikator, ako konstantu tak skopiruje obsah atributu*/
        if (act_token.typ == GENERAL) {
		  char *id = act_token.attribute.string;
		  tSymbol *tmp = h_Lookup(globTable,id);
		  
		  if (tmp == NULL || tmp->funcOrVar == false)
			return SemError();
		  
		  *idTyp=tmp->type;
		  pointerToData=&(tmp->value);
		  pointerToState=&(tmp->state);

		}
		else
		{
		    if (( pointerToData = malloc(sizeof(tAttribute)) )==NULL) {
		        InternalError("Internal error - Nie je k dispozicii dostatok pamate\n");
		        return false;
		    }
	        if (( pointerToState = malloc(sizeof(bool)) )==NULL) {
		        InternalError("Internal error - Nie je k dispozicii dostatok pamate\n");
		        return false;
		    }
		    *pointerToState = true;
            memcpy(pointerToData, &act_token.attribute, sizeof(act_token.attribute));

    		 if (act_token.typ == RES_TRUE) {
	             act_token.attribute.boolean=true;
	    	     *idTyp='b';
		     }
		     else if (act_token.typ == RES_FALSE) {
		         act_token.attribute.boolean=false;
		         *idTyp='b';
		     }
		     else if (act_token.typ == INTEGER) {
		         *idTyp='i';
		     }
		     else if (act_token.typ == REAL) { 
		         *idTyp='r';
		     }
		     else if (act_token.typ == STRING) {
		     *idTyp='s';
		     }
		}

	switch(tokenTyp){
		case 'b':
		  generateInstruction(list,I_WRITE_BOOL ,pointerToData , pointerToState , NULL);
		  return true;
		case 'i':
		  generateInstruction(list,I_WRITE_INT ,pointerToData , pointerToState , NULL);
		  return true;
		case 'r':
		  generateInstruction(list,I_WRITE_REAL ,pointerToData , pointerToState , NULL);
		  return true;
		case 's':
		  generateInstruction(list,I_WRITE_STRING ,pointerToData , pointerToState , NULL);
		  return true;

	}
	return true;
}

bool createWriteInstuctionFun(){
	char tokenTyp;
	char * idTyp=&tokenTyp;
	tAttribute* pointerToData;
	bool* pointerToState = NULL;
    
    /* Pozrie sa ci bude vypisovat konstantu alebo identifikator, ako konstantu tak skopiruje obsah atributu*/
        if (act_token.typ == GENERAL) {
		  char *id = act_token.attribute.string;
		  tSymbol *tmp = findSymbolInFunSem(id);
		  *idTyp=tmp->type;
		  pointerToData=&(tmp->value);
		  pointerToState=&(tmp->state);

		}
		else
		{
		    if (( pointerToData = malloc(sizeof(tAttribute)) )==NULL) {
		        InternalError("Internal error - Nie je k dispozicii dostatok pamate\n");
		        return false;
		    }
	        if (( pointerToState = malloc(sizeof(bool)) )==NULL) {
		        InternalError("Internal error - Nie je k dispozicii dostatok pamate\n");
		        return false;
		    }
		    *pointerToState = true;
            memcpy(pointerToData, &act_token.attribute, sizeof(act_token.attribute));

    		 if (act_token.typ == RES_TRUE) {
	             act_token.attribute.boolean=true;
	    	     *idTyp='b';
		     }
		     else if (act_token.typ == RES_FALSE) {
		         act_token.attribute.boolean=false;
		         *idTyp='b';
		     }
		     else if (act_token.typ == INTEGER) {
		         *idTyp='i';
		     }
		     else if (act_token.typ == REAL) { 
		         *idTyp='r';
		     }
		     else if (act_token.typ == STRING) {
		     *idTyp='s';
		     }
		}

	switch(tokenTyp){
		case 'b':
		  generateInstructionFun(h_Lookup(globTable,actFunction)->instructions,I_WRITE_BOOL ,pointerToData , pointerToState , NULL);
		  return true;
		case 'i':
		  generateInstructionFun(h_Lookup(globTable,actFunction)->instructions,I_WRITE_INT ,pointerToData , pointerToState , NULL);
		  return true;
		case 'r':
		  generateInstructionFun(h_Lookup(globTable,actFunction)->instructions,I_WRITE_REAL ,pointerToData , pointerToState , NULL);
		  return true;
		case 's':
		  generateInstructionFun(h_Lookup(globTable,actFunction)->instructions,I_WRITE_STRING ,pointerToData , pointerToState , NULL);
		  return true;

	}
	return true;
}



void listInitFun(tListOfInstrFun *L)
{
  L->First  = NULL;
  L->Last   = NULL;
  L->Active = NULL;
}

void listFreeFun(tListOfInstrFun *L)
{
  tListItem *ptr;
  while (L->First != NULL)
  {
    ptr = L->First;
    L->First = L->First->next;
    free(ptr);
  }
}

int listInsertLastFun(tListOfInstrFun *L, tInstr I)
{
  tListItem *newItem;
  if ((newItem = malloc(sizeof (tListItem))) == NULL)
	return 1;
	
  newItem->Instr = I;
  newItem->prev = L->Last;
  newItem->next = NULL;
  if (L->First == NULL)
     L->First = newItem;
  else
     L->Last->next=newItem;
  L->Last=newItem;
  
  return 0;
}

int listInsertNextFun(tListOfInstrFun *L, tInstr I)
{
	if (L->Active){
	
		tListItem *newItem;
	
		if ((newItem = malloc(sizeof(tListItem))) == NULL)
			return 1;
    
		newItem->Instr = I;
		
		newItem->next = L->Active->next;
		newItem->prev = L->Active;
		L->Active->next = newItem;
		
		if (L->Active != L->Last)						
			newItem->next->prev = newItem;
			else
				L->Last = newItem;
	}
	return 0;	
}

void generateInstructionNextFun(tListOfInstrFun *list, int instType, void *addr1, void *addr2, void *addr3)
{
   tInstr I;
   I.instType = instType;
   I.addr1 = addr1;
   I.addr2 = addr2;
   I.addr3 = addr3;
   listInsertNextFun(list, I);
}

void listFirstFun(tListOfInstrFun *L)
{
  L->Active = L->First;
}

void listLastFun(tListOfInstrFun *L)
{
  L->Active = L->Last;
}

void listNextFun(tListOfInstrFun *L)
{
  if (L->Active != NULL)
  L->Active = L->Active->next;
}

void listPreviousFun(tListOfInstrFun *L)
{
  if (L->Active != NULL)
  L->Active = L->Active->prev;
}

void *listGetPointerLastFun(tListOfInstrFun *L)
{
  return (void*) L->Last;
}

tInstr *listGetDataFun(tListOfInstrFun *L)
{
  return &(L->Active->Instr);
}

void generateInstructionFun(tListOfInstrFun *list, int instType, void *addr1, void *addr2, void *addr3)
{
   tInstr I;
   I.instType = instType;
   I.addr1 = addr1;
   I.addr2 = addr2;
   I.addr3 = addr3;
   listInsertLastFun(list, I);
}

// hlada v parametroch funkcie na zasobniku
tSymbol *findParam(char *name)
{	
	int i;
	if (s->STop->params == NULL)
		return NULL;
	
	for (i = 0; i < 50; i++)
	{		
		if (s->STop == NULL)
			return NULL;
					
		if (s->STop->params[i] && !strcmp(s->STop->params[i]->key,name))
		{
			return s->STop->params[i];		
		}
	}
					
	return NULL;
}

tSymbol *findSymbolInFun(char *name)
{
	tSymbol *sym;
	
	if ((sym = findParam(name)) == NULL)
		sym = lookInTables(name);
		
	return sym;
}

tSymbol *findSymbolInFunSem(char *name)
{
	tSymbol *sym;
		
	if ((sym = findParamSem(name)) == NULL)
		sym = lookInTablesSem(name);
		
	return sym;
}

// hlada v parametroch funkcie v tabulke symbolov
tSymbol *findParamSem(char *name)
{
	int i;
	
	for (i = 0; i < 50; i++)
	{
		if (h_Lookup(globTable,actFunction)->params[i] && !strcmp(h_Lookup(globTable,actFunction)->params[i]->key,name))
			return h_Lookup(globTable,actFunction)->params[i];			
	}				
	return NULL;
}

// skontroluje, ci ku kazdej forward deklaracii existuje definicia
bool checkForward()
{
	int i;
	tSymbol *tmp;

	for (i = 0; i < HSIZE; i++)
	{
		for (tmp = globTable->content[i]; tmp != NULL; tmp = tmp->next)
		{
			if (tmp->funcOrVar == false && tmp->defined == false)			// ak je to funkcia a nie je definovana
				return false;
		}
	}
	return true;
}

