 // parsing_precedence.c
 // Implementacia interpretu imperativneho jazyka IFJ14, 22.11.2014
 // Autor: Viktor Toth,xtothv01, FIT
 //        Peter Nagy,xnagyp01, FIT
 //        Dusan Zeliar,xzelia00, FIT
 //        Filip Mutnansky,xmutna00 , FIT
 //        Marek Ochodek,xochod01, FIT
 // Prelozene: gcc 4.8.2
 // .c subor parsing_precedence.c
 // precedencna syntakticka analyza vyrazov

//======================== FILES INCLUDED ===============================
#include "scanner.h"
#include "parsing.h"
#include <stdbool.h>
#include "error.h"
#include "interpret.h"
#include "sem.h"

//======================== GLOBAL VARIABLE ===============================

tListOfInstr *list;
TStack_tab *s;
char expType;

/*precedencna tabulka */
char rule[14][14] = {
/*               *    /    +    -    <    >    <=   >=   =    <>   i    (    )    $     */ 	
/* *  */	   {'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '>', '>', } , 
/* /  */	   {'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '>', '>', } , 
/* +  */	   {'<', '<', '>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '>', '>', } , 
/* -  */	   {'<', '<', '>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '>', '>', } , 
/* <  */	   {'<', '<', '<', '<', '>', '>', '>', '>', '>', '>', '<', '<', '>', '>', } , 
/* >  */	   {'<', '<', '<', '<', '>', '>', '>', '>', '>', '>', '<', '<', '>', '>', } , 
/* <= */	   {'<', '<', '<', '<', '>', '>', '>', '>', '>', '>', '<', '<', '>', '>', } , 
/* >= */	   {'<', '<', '<', '<', '>', '>', '>', '>', '>', '>', '<', '<', '>', '>', } , 
/* =  */	   {'<', '<', '<', '<', '>', '>', '>', '>', '>', '>', '<', '<', '>', '>', } , 
/* <> */	   {'<', '<', '<', '<', '>', '>', '>', '>', '>', '>', '<', '<', '>', '>', } , 
/* i  */	   {'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', 'x', 'x', '>', '>', } , 
/* (  */	   {'<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '=', 'x', } , 
/* )  */	   {'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', 'x', 'x', '>', '>', } , 
/* $  */	   {'<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', 'x', 'F', }  
};

//======================== FUNCTIONS ===============================
//STACK FUNCTIONS

void InitStack (tStack *S){
	S->Top = NULL;
	S->Terminal = NULL;
}

void DisposeStack (tStack *S) {
	/*vytvorenie pomocnej premennej tmp*/
	tStackItemPtr tmp=S->Top;
	
	/*pokial neukazuje TOP na NULL (koniec zoznamu), maze prvky */
	while (S->Top != NULL) {
		tmp = S->Top;
        S->Top = tmp->DownPtr;
        free(tmp);
	}
	return;  
}

bool PushTop (tStack *S, char znak ) {
    /*pomocny ukazatel*/
    tStackItemPtr new;
    /*alokovanie pamete, vlozenie hodnot do noveho prvku, posunutie ukazatela Top*/
    if (( new = malloc(sizeof(struct tStackItem)) )==NULL) {
		//CHYBA
		InternalError("Internal error - Nie je k dispozicii dostatok pamate\n");
		DisposeStack(S);
		return false;
	}
	/*-----------------------------------------------------------------*/
	if (znak != '$' || znak != 'P')
	{	// vkladanie terminalov na zasobnik
		if (act_token.typ == GENERAL)
		{
			tSymbol *tmp;
			// semanticka kontrola identifikatoru
			if (inFunction)
				tmp = findSymbolInFunSem(act_token.attribute.string);			
				else
					tmp = h_Lookup(globTable,act_token.attribute.string);
			
			if (tmp == NULL)
				return SemError();
			
			new->info.typ = tmp->type; 
			new->info.varOrCon = false;
			new->info.id = tmp->key;			
		} else if (act_token.typ == INTEGER)
			{
				new->info.varOrCon = true;
				new->info.typ = 'i';
				new->info.val.integer = act_token.attribute.integer;
			} else if (act_token.typ == REAL)
				{
					new->info.varOrCon = true;
					new->info.typ = 'r';
					new->info.val.real = act_token.attribute.real;
				} else if (act_token.typ == STRING)
					{
						new->info.varOrCon = true;
						new->info.typ = 's';
						new->info.val.string = act_token.attribute.string;
					} else if (act_token.typ == RES_FALSE)
						{
							new->info.varOrCon = true;
							new->info.typ = 'b';
							new->info.val.boolean = false;
						} else if (act_token.typ == RES_TRUE)
							{
								new->info.varOrCon = true;
								new->info.typ = 'b';
								new->info.val.boolean = true;
							}
	}
	/*-----------------------------------------------------------------*/
    new->symbol= znak;
    new->DownPtr= S->Top;
    new->UpPtr= NULL;
    /*Zretazenie zoznamu, nastavenie Top na new*/
    if (S->Top != NULL) S->Top->UpPtr= new;
    S->Top= new;
    
    return true;
}

bool PopTop (tStack *S, char * znak) {
	/*kontrola prazdneho zoznamu*/
	if (S->Top == NULL) {
		return false;
	}
	
	*znak=S->Top->symbol;
	/*pomocou pomocnej premennej tmp uvolnime prvy prvok*/
	tStackItemPtr tmp=S->Top;
	S->Top=tmp->DownPtr;
	/*
	if (tmp->saved_token != NULL) free(tmp->saved_token);
	*/
	if (S->Terminal == tmp) S->Terminal=NULL;
	free(tmp);
	if (S->Top != NULL)	S->Top->UpPtr=NULL;
	
	return true;
}	

/*funkcia vlozi start vyrazu "<" na zasobnik ako char 's'*/
bool InsertStartSymbol (tStack *S) {
    /*pomocny ukazatel*/
    tStackItemPtr new;
    
    /*alokovanie pamete, vlozenie hodnot do noveho prvku, posunutie ukazatela Top*/
    if (( new = malloc(sizeof(struct tStackItem)) )==NULL) {
		//CHYBA
		InternalError("Internal error - Nie je k dispozicii dostatok pamate\n");
		DisposeStack(S);
		return false;
	}

    /*nastav hodnoty polozky start*/
    new->symbol='s';
    new->UpPtr= S->Terminal->UpPtr;
    new->DownPtr= S->Terminal;
    /*nastav pointer Terminalu a prvku nad nim na novu polozku*/
    if (S->Terminal->UpPtr !=NULL){
        S->Terminal->UpPtr->DownPtr= new;
    }else S->Top=new;
    S->Terminal->UpPtr= new;

	return true;	
}

/*funkcia najse najvrchnejsi terminal a ukazatel nan ulozi do S->Terminal*/
bool FindTopTerminal(tStack *S) {
	S->Terminal=S->Top;
	while (S->Terminal != NULL) {
		if (S->Terminal->symbol != 'E' && S->Terminal->symbol != 'P'){
			return true;
		}
		S->Terminal=S->Terminal->DownPtr;
	}
	/*nenasiel ziadny terminal na zasobniku,chyba */
	ParseExpressionError("Syntactic Error in expression\n");
	DisposeStack(S);
	return false;
}

/*Funckia redukuje terminaly na zasobniku na NEtreminal, ak najde take pravidlo*/
int reduce(tStack *S) {
	/*pomocna premenna, ochovava vybrany symbol zo zasobniku*/
	char znak;
	/*---------------------------*/
	tTermInfo operand1 = S->Top->info;
	texpVal *valStackItem;
	tTermInfo nonTerminal;
	/*---------------------------*/
	PopTop(S,&znak);
	switch (znak){
		/* Prechadz pravidla pre gramatiku v poradi od zadu */
		case 'i':
		  /* E->i */
		  valStackItem = malloc(sizeof(texpVal));
		  valStackItem->typ = operand1.typ;
		  valStackItem->varOrCon = operand1.varOrCon;		  
		  
		  if (operand1.varOrCon == false)
		  {
			valStackItem->id = operand1.id;
		  }	else if (operand1.typ == 'i')
				{
					valStackItem->varOrCon = true;
					valStackItem->val.integer = operand1.val.integer;
				} else	if (operand1.typ == 'r')
					{
						valStackItem->varOrCon = true;
						valStackItem->val.real = operand1.val.real;
					} else if (operand1.typ == 's')
							{
								valStackItem->varOrCon = true;
								valStackItem->val.string = operand1.val.string;
							} else if (operand1.typ == 'b')
								{
									valStackItem->varOrCon = true;
									valStackItem->val.boolean = operand1.val.boolean;
								}	  
		  
		  // generovanie instrukcie na pridanie hodnoty alebo id na zasobnik
		  if (inFunction == false)
			generateInstruction(list,I_PUSHVAL ,valStackItem , NULL , NULL);
			else
				generateInstructionFun(h_Lookup(globTable,actFunction)->instructions,I_PUSHVAL ,valStackItem , NULL , NULL);
		 
		  if (!PopTop(S,&znak)) return 1;
		  if (znak != 's') return 1;
		  if (!PushTop(S,'E')) return 1;
		  /*------------------------------------------*/
		  S->Top->info = operand1;
		  expType = operand1.typ;
		  /*------------------------------------------*/
		  return 0; 
		case ')' :
          /* E-> (E) */
          /*-----------------------------------------*/
          nonTerminal = S->Top->info;
          /*-----------------------------------------*/
          if (!PopTop(S,&znak)) return 1;
          if (znak != 'E') return 1;
		  if (!PopTop(S,&znak)) return 1;
          if (znak != '(') return 1;
          if (!PopTop(S,&znak)) return 1;
		  if (znak != 's') return 1;
		  if (!PushTop(S,'E')) return 1;
		  /*------------------------------------------*/
		  S->Top->info = nonTerminal;
		  expType = nonTerminal.typ;
		  /*------------------------------------------*/
		  return 0;

		case 'E':
		  /* E-> E*E 
		     E-> E/E
		     E-> E+E
		     E-> E-E */
		   if (!PopTop(S,&znak)) return 1;
		   if (znak=='*' || znak=='/' || znak=='+' || znak=='-') {
			   /*--------------------------------------------------*/
			   char operator = znak;
			   tTermInfo operand2 = S->Top->info;			   
			   /*--------------------------------------------------*/
		   	   if (!PopTop(S,&znak)) return 1;
		   	   if (znak != 'E') return 1;
               if (!PopTop(S,&znak)) return 1;
		       if (znak != 's') return 1;
		       /*--------------------------------------------------*/
		       char rettype;
		       // TYPOVA KONTROLA
		       if (operator == '*' || operator == '+' || operator == '-' )
		       {
					if (operand1.typ == 'i' && operand2.typ == 'i')
						rettype = 'i';
						else if (operand1.typ == 's' && operand2.typ != 's')
							return 2;
							else if (operand1.typ != 's' && operand2.typ == 's')
								return 2;
								else if (operand1.typ == 'r' || operand2.typ == 'r')
									rettype = 'r';
									else if (operand1.typ == 's' && operand2.typ == 's')
									{
										rettype = 's';
										if (operator == '*' || operator == '-')
											return 2;
									}
			    } else if (operator == '/')
					{
						if (operand1.typ == 's' || operand2.typ == 's')
							return 2;
							else rettype = 'r';						
					}
					
		       /*--------------------------------------------------*/
		       /* push neterminalu E*/
		       if (!PushTop(S,'E')) return 1;
		       /*--------------------------------------------------*/
			       // generovanie instrukcii na operacie
			       if (inFunction == false)
			       {
						if (operator == '+')
							generateInstruction(list, I_PLUS, NULL, NULL, NULL);
							else if (operator == '*')
								generateInstruction(list, I_MUL, NULL, NULL, NULL);
								else if (operator == '-')
									generateInstruction(list, I_SUB, NULL, NULL, NULL);
									else if (operator == '/')
										generateInstruction(list, I_DIV, NULL, NULL, NULL);
					} else
						{
							if (operator == '+')
							generateInstructionFun(h_Lookup(globTable,actFunction)->instructions, I_PLUS, NULL, NULL, NULL);
							else if (operator == '*')
								generateInstructionFun(h_Lookup(globTable,actFunction)->instructions, I_MUL, NULL, NULL, NULL);
								else if (operator == '-')
									generateInstructionFun(h_Lookup(globTable,actFunction)->instructions, I_SUB, NULL, NULL, NULL);
									else if (operator == '/')
										generateInstructionFun(h_Lookup(globTable,actFunction)->instructions, I_DIV, NULL, NULL, NULL);
							
						}
					
					
		       S->Top->info.typ = rettype;
		       expType = rettype;
		       /*--------------------------------------------------*/
		       return 0;
		   }
		  /* E-> E<E 
		     E-> E>E
		     E-> E{E
		     E-> E}E 
		     E-> E=E
		     E-> E!E*/
		   else if (znak=='<' || znak=='>' || znak=='{' || znak=='}' || znak=='=' || znak=='!') {
			  /*--------------------------------------*/
			  char operator = znak;
			  tTermInfo operand2 = S->Top->info;
			  /*--------------------------------------*/
		   	  if (!PopTop(S,&znak)) return 1;
		   	  if (znak != 'E') return 1;
              if (!PopTop(S,&znak)) return 1;
		      if (znak != 's') return 1;
		      /*--------------------------------------------------*/
		      char rettype;
		      
		      if (operand1.typ == operand2.typ) 
			      rettype = 'b';
			      else
					return 2;
						  
		      /*--------------------------------------------------*/
		      /*push neterminalu P (podmienka) */
		      if (!PushTop(S,'P')) return 1;
		     
		     // generovanie instrukcii na porovnavanie
		     if (inFunction == false)
		     {
				  if (operator == '<')
					generateInstruction(list, I_LOWER, NULL, NULL, NULL);
					else if (operator == '>')
						generateInstruction(list, I_GREATER, NULL, NULL, NULL);
						else if (operator == '{')
							generateInstruction(list, I_LOWER_EQUAL, NULL, NULL, NULL);
							else if (operator == '}')
								generateInstruction(list, I_GREATER_EQUAL, NULL, NULL, NULL);	
								else if (operator == '=')
									generateInstruction(list, I_EQUAL, NULL, NULL, NULL);	
									else if (operator == '!')
										generateInstruction(list, I_NOT_EQUAL, NULL, NULL, NULL);	
			  } else
				{
					if (operator == '<')
					generateInstructionFun(h_Lookup(globTable,actFunction)->instructions, I_LOWER, NULL, NULL, NULL);
					else if (operator == '>')
						generateInstructionFun(h_Lookup(globTable,actFunction)->instructions, I_GREATER, NULL, NULL, NULL);
						else if (operator == '{')
							generateInstructionFun(h_Lookup(globTable,actFunction)->instructions, I_LOWER_EQUAL, NULL, NULL, NULL);
							else if (operator == '}')
								generateInstructionFun(h_Lookup(globTable,actFunction)->instructions, I_GREATER_EQUAL, NULL, NULL, NULL);	
								else if (operator == '=')
									generateInstructionFun(h_Lookup(globTable,actFunction)->instructions, I_EQUAL, NULL, NULL, NULL);	
									else if (operator == '!')
										generateInstructionFun(h_Lookup(globTable,actFunction)->instructions, I_NOT_EQUAL, NULL, NULL, NULL);	
					
				}
		      /*--------------------------------------------------*/
		      S->Top->info.typ = rettype;
		      expType = rettype;
		      /*--------------------------------------------------*/
		      return 0;
		   }
		   else return 1;
		 default: return 1;

	}
}

int charToIndex (char znak){
	switch (znak){
		case '*': return 0; 
		case '/': return 1; 
		case '+': return 2; 
		case '-': return 3;
		case '<': return 4;
		case '>': return 5;
		case '{': return 6;
		case '}': return 7;
		case '=': return 8;
		case '!': return 9;
		case 'i': return 10; 
		case '(': return 11; 
		case ')': return 12;
		case '$': return 13; 
		default: return 13; 
	}
	return 13;
}

char tokenToChar (){
	switch (act_token.typ){
		case OPERATOR_MUL: return '*'; 
		case OPERATOR_DIV: return '/'; 
		case OPERATOR_PLUS: return '+'; 
		case OPERATOR_MINUS: return '-';
		case OPERATOR_LESS: return '<';
		case OPERATOR_GREATER: return '>';
		case OPERATOR_LESS_EQUAL: return '{';
		case OPERATOR_GREATER_EQUAL: return '}';
		case OPERATOR_EQUAL: return '=';
		case OPERATOR_NOT_EQUAL: return '!';
		case GENERAL: 
		case INTEGER: 
		case REAL: 
		case STRING: 
		case RES_TRUE: 
		case RES_FALSE: return 'i'; 
		case OPERATOR_BRACKET_OPEN: return '('; 
		case OPERATOR_BRACKET_CLOSE: return ')'; 
		default: return '$'; 
	}
	return '$';
}

bool parseExpression () {
	/* kontrola existencie vyrazu*/
	if (!(act_token.typ == GENERAL || act_token.typ == INTEGER || act_token.typ == REAL ||
		 act_token.typ == STRING  || act_token.typ == RES_TRUE || act_token.typ == RES_FALSE || 
		 act_token.typ == OPERATOR_BRACKET_OPEN )){
		ParseExpressionError("Syntactic Error: missing expression\n"); 
	    return false;
	}

	/*deklaracia a inicializacia zasobnika*/
	int flag;
	tStack Stack;
	InitStack (&Stack);
	/*Vlozenie $ na zaciatok zasobnika */
	if (!PushTop(&Stack, '$')) return false;
	char act_symbol_stack,act_symbol_token;
	do {
		/*---------------------------------------------------*/
		// semanticke kontroly
		if (inFunction == false)
		{
			if ((act_token.typ == GENERAL) && (lookInTables(act_token.attribute.string) == NULL ))
				return SemError();
		} else
			{
				if ((act_token.typ == GENERAL) &&  findSymbolInFunSem(act_token.attribute.string) == NULL)
					return SemError();
			}
		/*---------------------------------------------------*/
		/*pomocna premenna pre aktualny symbol reprezentujuci token*/
		act_symbol_token=tokenToChar(&act_token);
		/*pokia nenasiel top terminal- chyba, konci cyklus*/
		if (!FindTopTerminal(&Stack)) return false;
		act_symbol_stack=Stack.Terminal->symbol;
		//pohlad to precedencnej tabulky
		switch( rule[charToIndex(act_symbol_stack)][charToIndex(act_symbol_token)] ) {
			case '=':
				/*vloz na vrch zasobnika aktualny symbol z tokenu*/
				if (!PushTop(&Stack, act_symbol_token) ) return false;
				/*nacita dalsi token*/
				if (!getNextToken ( source_file, &act_token)) {
					/*CHYBA, uvolnim zasobnik*/
					DisposeStack(&Stack);
					return false;
				}
				break;
			case '<': 
				/*vloz zalozku zaciatku spracovania */
				if (!InsertStartSymbol(&Stack))	return false;
				/*vloz na vrch zasobnika aktualny symbol z tokenu*/
				if (!PushTop(&Stack, act_symbol_token) ) return false;
				/*nacita dalsi token*/
				if (!getNextToken ( source_file, &act_token)) {
					/*CHYBA, uvolnim zasobnik*/
					DisposeStack(&Stack);
					return false;
				}
				break;
			case '>': 
				/*hladam pravidlo podla ktoreho vytvorim neterminal */
				flag = reduce(&Stack);
				if (flag == 1){
					/*CHYBA, uvolnim zasobnik*/
					ParseExpressionError("Syntactic Error in expression\n");
					DisposeStack(&Stack);
					return false;
				} else if (flag == 2)
					{
						SemError2();
						DisposeStack(&Stack);
						return false;
					}
				break;
			case 'F':
			  /* FINISHED, oba symboly $ $ */
			  break;
			default:
			/*inak x, znaci CHYBU */ 
			  ParseExpressionError("Syntactic Error in expression\n");
			  DisposeStack(&Stack);
			  return false;
		}

	} while ( !((act_symbol_token == '$') && (act_symbol_stack == '$')) ); 

	DisposeStack(&Stack);
	 
    if (inFunction == false)	   
	   generateInstruction(list, I_EVAL, NULL, NULL, NULL);
		   else
			  generateInstructionFun(h_Lookup(globTable,actFunction)->instructions, I_EVAL,NULL, NULL, NULL);
	return true;
} 



