 // interpret.c
 // Implementacia interpretu imperativneho jazyka IFJ14, 15.10.2014
 // Autor: Viktor Toth,xtothv01, FIT
 //        Peter Nagy,xnagyp01, FIT
 //        Dusan Zeliar,xzelia00, FIT
 //        Filip Mutnansky,xmutna00 , FIT
 //        Marek Ochodek,xochod01, FIT
 // Prelozene: gcc 4.8.2
 // .h subor scanner.h
 // implementacia interpretu

#include <ctype.h> 
#include <stdlib.h>
#include "scanner.h"
#include "parsing.h"
#include <stdbool.h>
#include "error.h"
#include "ial.h"
#include "sem.h" 
#include "interpret.h"
#include "functions.h"

TStack_val *stack;
tListOfInstr *list;
TStack_tab *s;
int numOfLabels = 0;

int interpret(tListOfInstr *list)
{
  // prechadza a vykonava instrukcie na instrukcnej paske
  listFirst(list);
  tInstr *I;
  while (list->Active && errflag==false)
  {
    I = listGetData(list);
    switch (I->instType)
    {
		case I_PUSHVAL:
			pushValueI((texpVal*)I->addr1); 
			break;
		
		case I_PLUS:
			plus();
			break;
			
		case I_MUL:
			mul();
			break;
			
		case I_DIV:
			divide();
			break;
			
		case I_SUB:
			sub();
			break;
		case I_ASSIGN:
			assign((char*)I->addr1);
			break;
			
		case I_WRITE_BOOL:
		    writeBool((tAttribute*)I->addr1,(bool*)I->addr2);
		    break;
		case I_WRITE_INT:
		    writeInt((tAttribute*)I->addr1,(bool*)I->addr2);
		    break;
		case I_WRITE_REAL:
		    writeReal((tAttribute*)I->addr1,(bool*)I->addr2);
		    break;
		case I_WRITE_STRING:
		    writeString((tAttribute*)I->addr1,(bool*)I->addr2);
		    break;
		case I_READLN_INT:
		    readlnInt((tAttribute*)I->addr1);
		    break;
		case I_READLN_REAL:
		    readlnReal((tAttribute*)I->addr1);
		    break;
		case I_READLN_STRING:
		    readlnString((tAttribute*)I->addr1);
		    break;
		case I_GREATER:
			greater();
			break;
		case I_LOWER:
			lower();
			break;
		case I_GREATER_EQUAL:
			greaterOrEqual();
			break;
		case I_LOWER_EQUAL:
			lowerOrEqual();
			break;
		case I_EQUAL:
			equal();
			break;
		case I_NOT_EQUAL:
			notEqual();
			break;
		case I_LAB:
			break;
		case I_GO_OR_STAY:
			goOrStay((char*)I->addr1);
			break;
		case I_POP_STACK:
			popStack();
			break;
		case I_GO_OR_STAY2:
			goOrStay2((char*)I->addr1);
			break;
	    case I_GO_BACK_OR_STAY:
	        goBackOrStay((char*)I->addr1);
	        break;
	    case I_PUSHPAR_INT:
			pushParInt((tSymbol *)I->addr1,(char *)I->addr2,(int *)I->addr3);
			break;
		case I_PUSHPAR_REAL:
			pushParReal((tSymbol *)I->addr1,(char *)I->addr2,(double *)I->addr3);
			break;
		case I_PUSHPAR_BOOL:
			pushParBool((tSymbol *)I->addr1,(char *)I->addr2,(bool *)I->addr3);
			break;
		case I_PUSHPAR_STRING:
			pushParString((tSymbol *)I->addr1,(char *)I->addr2,(char *)I->addr3);
			break;
		case I_CALL_FUN:
			interpretFun((char*)I->addr1);
			break;
		case I_PUSH_TABLE:
			pushTable((Thtab_t*)I->addr1, (tSymbol **)I->addr2);
			break;
		case I_EVAL:
			eval();
			break;
		case I_POP_TABLE:
			Stack_pop();
			break;
		case I_REVERSE_BOOL:
		    ReverseBool();
		    break;
		case I_LENGTH:
			callLength();
			break;
		case I_COPY:
			callCopy();
			break;
		case I_FIND:
			callFind();
			break;
		case I_SORT:
			callSort();
			break;
    }
    
     listNext(list);
  }
  return 0;
}
 
int pushValueI (texpVal *source)
{
	return Stack2_push(stack, source);
}

int plus()
{
	texpVal *tmp = malloc(sizeof(texpVal));
	
	bool operand1varOrCon = stack->STop->next->varOrCon;   // 0 - var, 1 - con
	bool operand2varOrCon = stack->STop->varOrCon;		   // 0 - var, 1 - con
	char operand1type = stack->STop->next->typ;
	char operand2type = stack->STop->typ;
	tAttribute operand1val;
	tAttribute operand2val;
	
	if (operand1varOrCon == true)
		operand1val = stack->STop->next->val;
		else
			{
				if (findSymbolInFun(stack->STop->next->id)->state == false)			// kontrola inicializacie
				{
					Uninitialized();
					return 1;
		  		}
				operand1val = findSymbolInFun(stack->STop->next->id)->value;
			}
			
	if (operand2varOrCon == true)
		operand2val = stack->STop->val;
		else
		{
			if (findSymbolInFun(stack->STop->id)->state == false)	// kontrola inicializacie
			{
				Uninitialized();
				return 1;
			}
			operand2val = findSymbolInFun(stack->STop->id)->value;	
		}

	if (operand1type == 'r' || operand2type == 'r')	
		tmp->typ = 'r';
		else 
			tmp->typ = 'i';
			
	tmp->varOrCon = true;
		
	if (operand1type == 'i' && operand2type == 'i'){
		tmp->typ = 'i';
		tmp->val.integer = operand1val.integer + operand2val.integer;
	}
		
	if (operand1type == 'i' && operand2type == 'r'){
		tmp->typ = 'r';
		tmp->val.real = operand1val.integer + operand2val.real;
	}
	
	if (operand1type == 'r' && operand2type == 'i'){
		tmp->typ = 'r';
		tmp->val.real = operand1val.real + operand2val.integer;
	}
		
	if (operand1type == 'r' && operand2type == 'r'){
		tmp->typ = 'r';
		tmp->val.real = operand1val.real + operand2val.real;
	}
		
	if (operand1type == 's' && operand2type == 's'){
		tmp->typ = 's';
		tmp->val.string = concatenate(operand1val.string,operand2val.string);
	}
	
	Stack2_pop(stack);		
	Stack2_pop(stack);
	Stack2_push(stack,tmp);
	return 0;
}
		
int mul()
{
	texpVal *tmp = malloc(sizeof(texpVal));
	bool operand1varOrCon = stack->STop->next->varOrCon;   // 0 - var, 1 - con
	bool operand2varOrCon = stack->STop->varOrCon;		   // 0 - var, 1 - con
	char operand1type = stack->STop->next->typ;
	char operand2type = stack->STop->typ;
	tAttribute operand1val;
	tAttribute operand2val;
	
	if (operand1varOrCon == true)
		operand1val = stack->STop->next->val;
		else
			{
				if (findSymbolInFun(stack->STop->next->id)->state == false)
				{
					Uninitialized();
					return 1;
				}
				operand1val = findSymbolInFun(stack->STop->next->id)->value;
			}
			
	if (operand2varOrCon == true)
		operand2val = stack->STop->val;
		else
		{
			if (findSymbolInFun(stack->STop->id)->state == false)
			{
				Uninitialized();
				return 1;
			}
			operand2val = findSymbolInFun(stack->STop->id)->value;	
		}

	if (operand1type == 'r' || operand2type == 'r')	
		tmp->typ = 'r';
		else 
			tmp->typ = 'i';
			
	tmp->varOrCon = true;
		
	if (operand1type == 'i' && operand2type == 'i')
		tmp->val.integer = operand1val.integer * operand2val.integer;
		
	if (operand1type == 'i' && operand2type == 'r')
		tmp->val.real = operand1val.integer * operand2val.real;
		
	if (operand1type == 'r' && operand2type == 'i')
		tmp->val.real = operand1val.real * operand2val.integer;
		
	if (operand1type == 'r' && operand2type == 'r')
		tmp->val.real = operand1val.real * operand2val.real;
	
	Stack2_pop(stack);		
	Stack2_pop(stack);
	Stack2_push(stack,tmp);
	return 0;
}

int divide()
{
	texpVal *tmp = malloc(sizeof(texpVal));
	
	bool operand1varOrCon = stack->STop->next->varOrCon;   // 0 - var, 1 - con
	bool operand2varOrCon = stack->STop->varOrCon;		   // 0 - var, 1 - con
	char operand1type = stack->STop->next->typ;
	char operand2type = stack->STop->typ;
	tAttribute operand1val;
	tAttribute operand2val;
	
	if (operand1varOrCon == true)
		operand1val = stack->STop->next->val;
		else
			{
				if (findSymbolInFun(stack->STop->next->id)->state == false)
				{
					Uninitialized();
					return 1;
				}
				operand1val = findSymbolInFun(stack->STop->next->id)->value;
			}
			
	if (operand2varOrCon == true)
		operand2val = stack->STop->val;
		else
		{
			if (findSymbolInFun(stack->STop->id)->state == false)
			{
				Uninitialized();
				return 1;
			}
			operand2val = findSymbolInFun(stack->STop->id)->value;	
		}

	tmp->typ = 'r';		
	tmp->varOrCon = true;
	
	if (operand1type == 'i' && operand2type == 'i')
	{
		if (operand2val.integer)
			tmp->val.real = (double)operand1val.integer/(double)operand2val.integer;
				else
				{
					ZeroDiv();
					return 1;
				}
	}
		
	if (operand1type == 'i' && operand2type == 'r')
	{
		if (operand2val.real)
			tmp->val.real = operand1val.integer / operand2val.real;
			else
				{
					ZeroDiv();
					return 1;
				}
	}
		
	if (operand1type == 'r' && operand2type == 'i')
	{
			if (operand2val.integer)
				tmp->val.real = operand1val.real / operand2val.integer;
				else
				{
					ZeroDiv();
					return 1;
				}
	}
		
	if (operand1type == 'r' && operand2type == 'r')
	{
		if (operand2val.real)
			tmp->val.real = operand1val.real / operand2val.real;
			else
			{
				ZeroDiv();
				return 1;
			}
	}
	
	Stack2_pop(stack);		
	Stack2_pop(stack);
	Stack2_push(stack,tmp);
	return 0;
}

int sub()
{
	texpVal *tmp = malloc(sizeof(texpVal));
	
	bool operand1varOrCon = stack->STop->next->varOrCon;   // 0 - var, 1 - con
	bool operand2varOrCon = stack->STop->varOrCon;		   // 0 - var, 1 - con
	char operand1type = stack->STop->next->typ;
	char operand2type = stack->STop->typ;
	tAttribute operand1val;
	tAttribute operand2val;
	
	if (operand1varOrCon == true)
		operand1val = stack->STop->next->val;
		else
			{
				if (findSymbolInFun(stack->STop->next->id)->state == false)
				{
					Uninitialized();
					return 1;
				}
				operand1val = findSymbolInFun(stack->STop->next->id)->value;
			}
			
	if (operand2varOrCon == true)
		operand2val = stack->STop->val;
		else
		{
			if (findSymbolInFun(stack->STop->id)->state == false)
			{
				Uninitialized();
				return 1;
			}
			operand2val = findSymbolInFun(stack->STop->id)->value;	
		}
		
	if (operand1type == 'r' || operand2type == 'r')	
		tmp->typ = 'r';
		else 
			tmp->typ = 'i';
			
	tmp->varOrCon = true;
		
	if (operand1type == 'i' && operand2type == 'i')
		tmp->val.integer = operand1val.integer - operand2val.integer;
		
	if (operand1type == 'i' && operand2type == 'r')
		tmp->val.real = operand1val.integer - operand2val.real;
		
	if (operand1type == 'r' && operand2type == 'i')
		tmp->val.real = operand1val.real - operand2val.integer;
		
	if (operand1type == 'r' && operand2type == 'r')
		tmp->val.real = operand1val.real - operand2val.real;
	
	Stack2_pop(stack);		
	Stack2_pop(stack);
	Stack2_push(stack,tmp);
	return 0;
}

int greater()
{
	texpVal *tmp = malloc(sizeof(texpVal));
	
	bool operand1varOrCon = stack->STop->next->varOrCon;   // 0 - var, 1 - con
	bool operand2varOrCon = stack->STop->varOrCon;		   // 0 - var, 1 - con
	char operand1type = stack->STop->next->typ;
	char operand2type = stack->STop->typ;
	tAttribute operand1val;
	tAttribute operand2val;
	
	if (operand1varOrCon == true)
		operand1val = stack->STop->next->val;
		else
			{
				if (findSymbolInFun(stack->STop->next->id)->state == false)
				{
					Uninitialized();
					return 1;
				}
				operand1val = findSymbolInFun(stack->STop->next->id)->value;
			}
			
	if (operand2varOrCon == true)
		operand2val = stack->STop->val;
		else
		{
			if (findSymbolInFun(stack->STop->id)->state == false)
			{
				Uninitialized();
				return 1;
			}
			operand2val = findSymbolInFun(stack->STop->id)->value;	
		}

	tmp->typ = 'b';
	tmp->varOrCon = true;
		
	if (operand1type == 'i' && operand2type == 'i')
		tmp->val.boolean = operand1val.integer > operand2val.integer;
		
	if (operand1type == 'r' && operand2type == 'r')
		tmp->val.boolean = operand1val.real > operand2val.real;
		
	if (operand1type == 'b' && operand2type == 'b')
	{
		tmp->val.boolean = operand1val.boolean > operand2val.boolean;
	}
		
	if (operand1type == 's' && operand2type == 's')
	{
		int result = strcmp(operand1val.string,operand2val.string);
		
		if (result > 0)
			tmp->val.boolean = true;
			else
				tmp->val.boolean = false;
	}

	Stack2_pop(stack);		
	Stack2_pop(stack);
	Stack2_push(stack,tmp);
	return 0;
}

int greaterOrEqual()
{
	texpVal *tmp = malloc(sizeof(texpVal));
	
	bool operand1varOrCon = stack->STop->next->varOrCon;   // 0 - var, 1 - con
	bool operand2varOrCon = stack->STop->varOrCon;		   // 0 - var, 1 - con
	char operand1type = stack->STop->next->typ;
	char operand2type = stack->STop->typ;
	tAttribute operand1val;
	tAttribute operand2val;
	
	if (operand1varOrCon == true)
		operand1val = stack->STop->next->val;
		else
		{
				if (findSymbolInFun(stack->STop->next->id)->state == false)
				{
					Uninitialized();
					return 1;
				}
				operand1val = findSymbolInFun(stack->STop->next->id)->value;
			}
			
	if (operand2varOrCon == true)
		operand2val = stack->STop->val;
		else
		{
			if (findSymbolInFun(stack->STop->id)->state == false)
			{
				Uninitialized();
				return 1;
			}
			operand2val = findSymbolInFun(stack->STop->id)->value;	
		}

	tmp->typ = 'b';
	tmp->varOrCon = true;	
		
	if (operand1type == 'i' && operand2type == 'i')
		tmp->val.boolean = operand1val.integer >= operand2val.integer;
		
	if (operand1type == 'r' && operand2type == 'r')
		tmp->val.boolean = operand1val.real >= operand2val.real;
	
	if (operand1type == 'b' && operand2type == 'b')
		tmp->val.boolean = operand1val.boolean >= operand2val.boolean;
		
	if (operand1type == 's' && operand2type == 's')
	{
		int result = strcmp(operand1val.string,operand2val.string);
		
		if (result >= 0)
			tmp->val.boolean = true;
			else
				tmp->val.boolean = false;
	}

	Stack2_pop(stack);		
	Stack2_pop(stack);
	Stack2_push(stack,tmp);
	return 0;
}

int lower()
{
	texpVal *tmp = malloc(sizeof(texpVal));
	
	bool operand1varOrCon = stack->STop->next->varOrCon;   // 0 - var, 1 - con
	bool operand2varOrCon = stack->STop->varOrCon;		   // 0 - var, 1 - con
	char operand1type = stack->STop->next->typ;
	char operand2type = stack->STop->typ;
	tAttribute operand1val;
	tAttribute operand2val;
	
		
	if (operand1varOrCon == true)
		operand1val = stack->STop->next->val;
		else
		{
				if (findSymbolInFun(stack->STop->next->id)->state == false)
				{
					Uninitialized();
					return 1;
				}
				operand1val = findSymbolInFun(stack->STop->next->id)->value;
		}
			
	if (operand2varOrCon == true)
		operand2val = stack->STop->val;
		else
		{
			if (findSymbolInFun(stack->STop->id)->state == false)
			{
				Uninitialized();
				return 1;
			}
			operand2val = findSymbolInFun(stack->STop->id)->value;	
		}

	tmp->typ = 'b';
	tmp->varOrCon = true;
		
	if (operand1type == 'i' && operand2type == 'i')
		tmp->val.boolean = operand1val.integer < operand2val.integer;
		
	if (operand1type == 'r' && operand2type == 'r')
		tmp->val.boolean = operand1val.real < operand2val.real;
	
	if (operand1type == 'b' && operand2type == 'b')
		tmp->val.boolean = operand1val.boolean < operand2val.boolean;
		
	if (operand1type == 's' && operand2type == 's')
	{
		int result = strcmp(operand1val.string,operand2val.string);
		
		if (result < 0)
			tmp->val.boolean = true;
			else
				tmp->val.boolean = false;
	}

	Stack2_pop(stack);		
	Stack2_pop(stack);
	Stack2_push(stack,tmp);
	return 0;
}

int lowerOrEqual()
{
	texpVal *tmp = malloc(sizeof(texpVal));
	
	bool operand1varOrCon = stack->STop->next->varOrCon;   // 0 - var, 1 - con
	bool operand2varOrCon = stack->STop->varOrCon;		   // 0 - var, 1 - con
	char operand1type = stack->STop->next->typ;
	char operand2type = stack->STop->typ;
	tAttribute operand1val;
	tAttribute operand2val;
	
	if (operand1varOrCon == true)
		operand1val = stack->STop->next->val;
		else
		{
				if (findSymbolInFun(stack->STop->next->id)->state == false)
				{
					Uninitialized();
					return 1;
				}
				operand1val = findSymbolInFun(stack->STop->next->id)->value;
		}
			
	if (operand2varOrCon == true)
		operand2val = stack->STop->val;
		else
		{
			if (findSymbolInFun(stack->STop->id)->state == false)
			{
				Uninitialized();
				return 1;
			}
			operand2val = findSymbolInFun(stack->STop->id)->value;	
		}

	tmp->typ = 'b';
	tmp->varOrCon = true;
		
	if (operand1type == 'i' && operand2type == 'i')
		tmp->val.boolean = operand1val.integer <= operand2val.integer;
		
	if (operand1type == 'r' && operand2type == 'r')
		tmp->val.boolean = operand1val.real <= operand2val.real;
	
	if (operand1type == 'b' && operand2type == 'b')
		tmp->val.boolean = operand1val.boolean <= operand2val.boolean;
		
	if (operand1type == 's' && operand2type == 's')
	{
		int result = strcmp(operand1val.string,operand2val.string);
		
		if (result <= 0)
			tmp->val.boolean = true;
			else
				tmp->val.boolean = false;
	}
	
	Stack2_pop(stack);		
	Stack2_pop(stack);
	Stack2_push(stack,tmp);
	return 0;
}

int equal()
{
	texpVal *tmp = malloc(sizeof(texpVal));
	
	bool operand1varOrCon = stack->STop->next->varOrCon;   // 0 - var, 1 - con
	bool operand2varOrCon = stack->STop->varOrCon;		   // 0 - var, 1 - con
	char operand1type = stack->STop->next->typ;
	char operand2type = stack->STop->typ;
	tAttribute operand1val;
	tAttribute operand2val;
	
	if (operand1varOrCon == true)
		operand1val = stack->STop->next->val;
		else
		{
				if (findSymbolInFun(stack->STop->next->id)->state == false)
				{
					Uninitialized();
					return 1;
				}
				operand1val = findSymbolInFun(stack->STop->next->id)->value;
		}
			
	if (operand2varOrCon == true)
		operand2val = stack->STop->val;
		else
		{
			if (findSymbolInFun(stack->STop->id)->state == false)
			{
				Uninitialized();
				return 1;
			}
			operand2val = findSymbolInFun(stack->STop->id)->value;	
		}

	tmp->typ = 'b';
	tmp->varOrCon = true;
		
	if (operand1type == 'i' && operand2type == 'i')
		tmp->val.boolean = operand1val.integer == operand2val.integer;
		
	if (operand1type == 'r' && operand2type == 'r')
		tmp->val.boolean = operand1val.real == operand2val.real;
		
	if (operand1type == 's' && operand2type == 's')
	{
		int result = strcmp(operand1val.string,operand2val.string);
		
		if (result == 0)
			tmp->val.boolean = true;
			else
				tmp->val.boolean = false;
	}
		
	if (operand1type == 'b' && operand2type == 'b')
		tmp->val.boolean = operand1val.boolean == operand2val.boolean;

	Stack2_pop(stack);		
	Stack2_pop(stack);
	Stack2_push(stack,tmp);
	return 0;
}

int notEqual()
{
	texpVal *tmp = malloc(sizeof(texpVal));
	
	bool operand1varOrCon = stack->STop->next->varOrCon;   // 0 - var, 1 - con
	bool operand2varOrCon = stack->STop->varOrCon;		   // 0 - var, 1 - con
	char operand1type = stack->STop->next->typ;
	char operand2type = stack->STop->typ;
	tAttribute operand1val;
	tAttribute operand2val;
	
	if (operand1varOrCon == true)
		operand1val = stack->STop->next->val;
		else
		{
				if (findSymbolInFun(stack->STop->next->id)->state == false)
				{
					Uninitialized();
					return 1;
				}
				operand1val = findSymbolInFun(stack->STop->next->id)->value;
		}
			
	if (operand2varOrCon == true)
		operand2val = stack->STop->val;
		else
		{
			if (findSymbolInFun(stack->STop->id)->state == false)
			{
				Uninitialized();
				return 1;
			}
			operand2val = findSymbolInFun(stack->STop->id)->value;	
		}

	tmp->typ = 'b';
	tmp->varOrCon = true;	
		
	if (operand1type == 'i' && operand2type == 'i')
		tmp->val.boolean = operand1val.integer != operand2val.integer;
		
	if (operand1type == 'r' && operand2type == 'r')
		tmp->val.boolean = operand1val.real != operand2val.real;
	
	if (operand1type == 'b' && operand2type == 'b')
		tmp->val.boolean = operand1val.boolean != operand2val.boolean;
		
	if (operand1type == 's' && operand2type == 's')
	{
		int result = strcmp(operand1val.string,operand2val.string);
		
		if (result != 0)
			tmp->val.boolean = true;
			else
				tmp->val.boolean = false;
	}

	Stack2_pop(stack);		
	Stack2_pop(stack);
	Stack2_push(stack,tmp);
	return 0;
}

int assign(char *id)
{	
	tSymbol *sym = findSymbolInFun(id);
	bool operandRvarOrCon = stack->STop->varOrCon;   // 0 - var, 1 - con
	char operandLtype = sym->type;
	char operandRtype = stack->STop->typ;
	tAttribute operandRval;
		
	if (operandRvarOrCon == true)
	{
		operandRval = stack->STop->val;
	}
		else
		{
			if (findSymbolInFun(stack->STop->id)->state == false)
			{
				Uninitialized();
				return 1;
			}
			
			operandRval = findSymbolInFun(stack->STop->id)->value;	
		}
		
	sym->state = true;		// nastavenie inicializacie
	
	if (operandRtype == 'i' && operandLtype == 'i')
		sym->value.integer = operandRval.integer;
		if (operandRtype == 'r' && operandLtype == 'r')
			sym->value.real = operandRval.real;
			if (operandRtype == 'i' && operandLtype == 'r')
				sym->value.real = operandRval.integer;
				if (operandRtype == 'b' && operandLtype == 'b')
					sym->value.boolean = operandRval.boolean;
					if (operandRtype == 's' && operandLtype == 's')
						sym->value.string = operandRval.string;
						
	Stack2_pop(stack);
	return 0;
}

void writeBool(tAttribute * pointerToData, bool *pointerToState){
	if (*pointerToState == false)
	{
		Uninitialized();
		return;
	}
	
	bool hodnota=pointerToData->boolean;
	if (hodnota) printf("%s","TRUE" );
	else printf("%s","FALSE" );
 
}

void writeInt(tAttribute * pointerToData, bool *pointerToState){
	if (*pointerToState == false)
	{
		Uninitialized();
		return;
	}
	
	printf("%i",pointerToData->integer );
}

void writeReal(tAttribute * pointerToData, bool *pointerToState){
	if (*pointerToState == false)
	{
		Uninitialized();
		return;
	}
	
	printf("%g",pointerToData->real );
}

void writeString(tAttribute * pointerToData, bool *pointerToState){
	if (*pointerToState == false)
	{
		Uninitialized();
		return;
	}
	
	printf("%s",pointerToData->string );
}

void readlnInt(tAttribute * pointerToData){
	int i=0;
	int c=getchar();
	while(isspace(c)) c=getchar();

	char *ptr;
	char content[20];
    while(isdigit(c)) {
    	content[i]=c;
    	i++;
    	c=getchar();
    }
    ungetc(c, stdin);

    content[i]='\0';
    //vycisti vstup po koniec radku
        while ( getchar() != '\n' ) ;

    pointerToData->integer = strtol(content, &ptr, 10);
    if (!*ptr) return;
    else InterpretReadError("failed to read integer from input\n");
    return;

}

void readlnReal(tAttribute * pointerToData){
    if (scanf("%lf", &(pointerToData->real)) == 1) {
        //vycisti vstup po koniec radku
        while ( getchar() != '\n' ) ;
    } else {
        InterpretReadError("failed to read real from input\n");
        while ( getchar() != '\n' ) ;
    }
}

void readlnString(tAttribute * pointerToData){
    int bufferSize=64;
    int contentSize = 0; 
    char *content;
    //alokacia zakladneho bufferu
    if (( content = malloc(sizeof(char) * bufferSize) )==NULL) {    
        InternalError("Internal error - Nie je k dispozicii dostatok pamate\n");
        return;
    }
    int c;
    //pokial nenarazim na novy radok alebo eof
    while((c = getchar()) != EOF && c!= '\n')
    {
        content[contentSize]=c;
        contentSize++;
        
        //ak by uz buffer nestacil (index +1 + 1 (bit na '\0'))
        if (contentSize+2 ==bufferSize ){
    	    bufferSize*=2;
    	    char *old=content;
    	    if (( content=realloc(content,bufferSize) )==NULL) {
    		    InternalError("Internal error - Nie je k dispozicii dostatok pamate\n");
    		    free (old);
    		    return; 
            }
        }
    }
    content[contentSize]='\0';
    // uvolni stary retazec, pokial nejaky obsahoval
    if (pointerToData->string != NULL) free (pointerToData->string); 
    pointerToData->string=content;
    
}

char * concatenate(char * s1, char * s2) {
	char * new;
	if (( new = malloc(strlen(s1)+strlen(s2)+1) )==NULL) { 
		InternalError("Internal error - Nie je k dispozicii dostatok pamate\n");
		return NULL;
	}
	strcpy(new, s1);
	strcat(new, s2);
	return new;
}

char* generateLabelName()
{
	char number[10];
	int value = numOfLabels;
	sprintf(number, "%d", value);
	char *name = concatenate("$",number);
	return name;
}


int goOrStay(char *name)		// ak je vrchol false skoci
{
	if (stack->STop->val.boolean == false)
	{
		tInstr *I;
        listNext(list); 
        while (list->Active && errflag==false)
        {
            I = listGetData(list);
            if (I->addr1 && !strcmp((char *)I->addr1,name))		//porovna retazce
    		return 0;
				
    	 listNext(list);     
        }	
	return 1;
	}
			
	return 0;
}

int goOrStayFun(char *name, tListOfInstrFun *listFun)		// ak je vrchol false skoci
{
	if (stack->STop->val.boolean == false)
	{
		tInstr *I;
        listNextFun(listFun); 
        while (listFun->Active && errflag==false)
        {
            I = listGetDataFun(listFun);
            if (I->addr1 && !strcmp((char *)I->addr1,name))		//porovna retazce
    		return 0;
				
    	 listNextFun(listFun);     
        }	
	return 1;
	}
			
	return 0;
}

int goOrStay2(char *name)		// ak je vrchol true skoci
{
	if (stack->STop->val.boolean == true)
	{
		Stack2_pop(stack);	
		tInstr *I;
        listNext(list); 
        while (list->Active && errflag==false)
        {
            I = listGetData(list);
            if (I->addr1 && !strcmp((char *)I->addr1,name))		//porovna retazce
    		return 0;
				
    	 listNext(list);     
        }	
	return 1;
	}else Stack2_pop(stack);
			
	return 0;
}

int goOrStay2Fun(char *name, tListOfInstrFun *listFun)		// ak je vrchol true skoci
{
	if (stack->STop->val.boolean == true)
	{
		Stack2_pop(stack);	
		tInstr *I;
        listNextFun(listFun); 
        while (listFun->Active && errflag==false)
        {
            I = listGetDataFun(listFun);
            if (I->addr1 && !strcmp((char *)I->addr1,name))		//porovna retazce
    		return 0;
				
    	 listNextFun(listFun);     
        }	
	return 1;
	}else Stack2_pop(stack);
			
	return 0;
}


int goBackOrStay(char *name){
	if (stack->STop->val.boolean == true){
		Stack2_pop(stack);	
        tInstr *I;
        listPrevious(list);
        while (list->Active && errflag==false)
        {
            I = listGetData(list);
            if (I->addr1 && !strcmp((char *)I->addr1,name))		//porovna retazce
	    	return 0;
				
    	 listPrevious(list);
        }
	    return 1;
	}
	else{
	Stack2_pop(stack);	
	} 
	return 0;

}

int goBackOrStayFun(char *name, tListOfInstrFun *listFun){
	if (stack->STop->val.boolean == true){
		Stack2_pop(stack);	
        tInstr *I;
        listPreviousFun(listFun);
        while (listFun->Active && errflag==false)
        {
            I = listGetDataFun(listFun);
            if (I->addr1 && !strcmp((char *)I->addr1,name))		//porovna retazce
	    	return 0;
				
    	 listPreviousFun(listFun);
        }
	    return 1;
	}
	else{
	Stack2_pop(stack);	
	} 
	return 0;

}

int popStack()
{
	Stack2_pop(stack);	
	return 0;
}

int pushParInt(tSymbol *dest, char *source, int *val)
{	
	if (source != NULL)
	{	
		tSymbol *tmp2 = findSymbolInFun(source);
				
		if (tmp2->state == false)
		{
			Uninitialized();
			return 1;
		}
		
		dest->value.integer = tmp2->value.integer;
	
	} else	
		dest->value.integer = *val;
					
	dest->state = true;	
	return 0;
}

int pushParReal(tSymbol *dest, char *source, double *val)
{
	if (source != NULL)
	{	
		tSymbol *tmp2 = findSymbolInFun(source);
		
		if (tmp2->state == false)
		{
			Uninitialized();
			return 1;
		}
				
		dest->value.real = tmp2->value.real;
	} else
		{
			dest->value.real = *val;
		}
					
	dest->state = true;
	
	return 0;
}

int pushParBool(tSymbol *dest, char *source, bool *val)
{
	if (source != NULL)
	{	
		tSymbol *tmp2 = findSymbolInFun(source);
		
		if (tmp2->state == false)
		{
			Uninitialized();
			return 1;
		}
				
		dest->value.boolean = tmp2->value.boolean;
	} else
		{
			dest->value.boolean = *val;
		}
					
	dest->state = true;
	
	return 0;
}

int pushParString(tSymbol *dest, char *source, char *val)
{
	if (source != NULL)
	{			
		tSymbol *tmp2 = findSymbolInFun(source);
		
		if (tmp2->state == false)
		{
			Uninitialized();
			return 1;
		}		
		dest->value.string = tmp2->value.string;
	} else
		{
			dest->value.string = val;
		}
					
	dest->state = true;
	
	return 0;
}

void ReverseBool(){
	stack->STop->val.boolean = ! (stack->STop->val.boolean);
	return;
}

// prechadza instrukcie funkcie a pridava ich na hlavny zoznam instrukcii
int interpretFun(char *name)
{
  tListOfInstrFun *listFun;
  
  listFun = h_Lookup(globTable,name)->instructions;
  
  listLastFun(listFun);
  tInstr *I;
  while (listFun->Active && errflag==false)
  {
    I = listGetDataFun(listFun);
    switch (I->instType)
    {
		case I_PUSHVAL:
			generateInstructionNext(list,I_PUSHVAL,(texpVal *)I->addr1,NULL,NULL);
			break;
		
		case I_PLUS:
			generateInstructionNext(list,I_PLUS,NULL,NULL,NULL);
			break;
			
		case I_MUL:
			generateInstructionNext(list,I_MUL,NULL,NULL,NULL);
			break;
			
		case I_DIV:
			generateInstructionNext(list,I_DIV,NULL,NULL,NULL);
			break;
			
		case I_SUB:
			generateInstructionNext(list,I_SUB,NULL,NULL,NULL);
			break;
		case I_ASSIGN:
			generateInstructionNext(list,I_ASSIGN,(char*)I->addr1,NULL,NULL);
			break;
		case I_WRITE_BOOL:
			generateInstructionNext(list,I_WRITE_BOOL,(tAttribute*)I->addr1,(bool*)I->addr2,NULL);
		    break;
		case I_WRITE_INT:
			generateInstructionNext(list,I_WRITE_INT,(tAttribute*)I->addr1,(bool*)I->addr2,NULL);
		    break;
		case I_WRITE_REAL:
		    generateInstructionNext(list,I_WRITE_REAL,(tAttribute*)I->addr1,(bool*)I->addr2,NULL);
		    break;
		case I_WRITE_STRING:
		    generateInstructionNext(list,I_WRITE_STRING,(tAttribute*)I->addr1,(bool*)I->addr2,NULL);
		    break;
		case I_READLN_INT:
			generateInstructionNext(list,I_READLN_INT,(tAttribute*)I->addr1,NULL,NULL);
		    break;
		case I_READLN_REAL:
		    generateInstructionNext(list,I_READLN_REAL,(tAttribute*)I->addr1,NULL,NULL);
		    break;
		case I_READLN_STRING:
		    generateInstructionNext(list,I_READLN_STRING,(tAttribute*)I->addr1,NULL,NULL);
		    break;
		case I_GREATER:
			generateInstructionNext(list,I_GREATER,NULL,NULL,NULL);
			break;
		case I_LOWER:
			generateInstructionNext(list,I_LOWER,NULL,NULL,NULL);
			break;
		case I_GREATER_EQUAL:
			generateInstructionNext(list,I_GREATER_EQUAL,NULL,NULL,NULL);
			break;
		case I_LOWER_EQUAL:
			generateInstructionNext(list,I_LOWER_EQUAL,NULL,NULL,NULL);
			break;
		case I_EQUAL:
			generateInstructionNext(list,I_EQUAL,NULL,NULL,NULL);
			break;
		case I_NOT_EQUAL:
			generateInstructionNext(list,I_NOT_EQUAL,NULL,NULL,NULL);
			break;
		case I_LAB:
			generateInstructionNext(list,I_LAB,(char*)I->addr1,NULL,NULL);
			break;
		case I_GO_OR_STAY:
			generateInstructionNext(list,I_GO_OR_STAY,(char*)I->addr1,NULL,NULL);
			break;
		case I_POP_STACK:
			generateInstructionNext(list,I_POP_STACK,NULL,NULL,NULL);
			break;
		case I_GO_OR_STAY2:
			generateInstructionNext(list,I_GO_OR_STAY2,(char*)I->addr1,NULL,NULL);
			break;
	    case I_GO_BACK_OR_STAY:
	        generateInstructionNext(list,I_GO_OR_STAY,(char*)I->addr1,NULL,NULL);
	        break;
	    case I_PUSHPAR_INT:
			generateInstructionNext(list,I_PUSHPAR_INT,(tSymbol*)I->addr1,(char *)I->addr2,(int *)I->addr3);
			break;
		case I_PUSHPAR_REAL:
			generateInstructionNext(list,I_PUSHPAR_REAL,(tSymbol*)I->addr1,(char *)I->addr2,(double *)I->addr3);
			break;
		case I_PUSHPAR_BOOL:
			generateInstructionNext(list,I_PUSHPAR_BOOL,(tSymbol*)I->addr1,(char *)I->addr2,(bool *)I->addr3);
			break;
		case I_PUSHPAR_STRING:
			generateInstructionNext(list,I_PUSHPAR_STRING,(tSymbol*)I->addr1,(char *)I->addr2,(char *)I->addr3);
			break;
		case I_CALL_FUN:
			generateInstructionNext(list,I_CALL_FUN,(char*)I->addr1,NULL,NULL);
			break;
		case I_PUSH_TABLE:
			generateInstructionNext(list,I_PUSH_TABLE,(Thtab_t*)I->addr1,(tSymbol **)I->addr2,NULL);
			break;
		case I_EVAL:
			generateInstructionNext(list,I_EVAL,NULL,NULL,NULL);
			break;
		case I_POP_TABLE:
			generateInstructionNext(list,I_POP_TABLE,NULL,NULL,NULL);
			break;
		case I_REVERSE_BOOL:
		    generateInstructionNext(list,I_REVERSE_BOOL,NULL,NULL,NULL);
		    break;
		case I_LENGTH:
		    generateInstructionNext(list,I_LENGTH,NULL,NULL,NULL);
		    break;
		case I_COPY:
			generateInstructionNext(list,I_COPY,NULL,NULL,NULL);
			break;
		case I_FIND:
			generateInstructionNext(list,I_FIND,NULL,NULL,NULL);
			break;
		case I_SORT:
			generateInstructionNext(list,I_SORT,NULL,NULL,NULL);
			break;
    }
    
     listPreviousFun(listFun);
  }
    
  return 0;
}

int pushTable(Thtab_t *tab, tSymbol **par)
{
	int i;
	Stack_push(copyTable(tab));
	
	for (i = 0; par[i]!=NULL; i++)
	{
		s->STop->params[i] = malloc(sizeof(tSymbol));
		strncpy(s->STop->params[i]->key, par[i]->key, strlen(par[i]->key));	// kopia key do polozky
				
		s->STop->params[i]->type = par[i]->type;
		s->STop->params[i]->funcOrVar = par[i]->funcOrVar;
		s->STop->params[i]->state = par[i]->state;
		s->STop->params[i]->value = par[i]->value;
	}
	
	return 0;
}

int eval()
{
	if (stack->STop->varOrCon == false)
	{
		tSymbol *sym = findSymbolInFun(stack->STop->id);
		texpVal *tmp = malloc(sizeof(texpVal));
		tmp->typ = sym->type;
		tmp->varOrCon = true;
		tmp->val = sym->value;
		Stack2_pop(stack);		
		Stack2_push(stack,tmp); 
	}
	
	return 0;
}

// importuje vstavane funkcie do tabulky symbolov
int importFunctions()
{
	//LENGTH
	tSymbol *length;
	
	if ((length = h_Insert(globTable,"length")) == NULL)
		return 1;
	
	length->type = 'i';
	length->funcOrVar = false;
	length->defined = true;
	
	if ((length->funcParams = malloc(sizeof(char))) == NULL)
	{
		InternalError("Allocation error\n");
		return 1;
	}
	
	if ((length->params[0] = malloc(sizeof(tSymbol))) == NULL)
	{
		InternalError("Allocation error\n");
		return 1;
	}
				
	length->funcParams[0] = 's';
		
	length->funcTab = initTable(HSIZE);
	
	if ((length->instructions = malloc(sizeof(tListOfInstrFun))) == NULL)
	{
		InternalError("Allocation error\n");
		return 1;
	}
	
	listInitFun(length->instructions);
		
	//COPY
	
	tSymbol *copy;
	
	if ((copy = h_Insert(globTable,"copy")) == NULL)
		return 1;
	
	copy->type = 's';
	copy->funcOrVar = false;
	copy->defined = true;
	
	if ((copy->funcParams = malloc(3*sizeof(char))) == NULL)
	{
		InternalError("Allocation error\n");
		return 1;
	}
	
	if ((copy->params[0] = malloc(sizeof(tSymbol))) == NULL)
	{
		InternalError("Allocation error\n");
		return 1;
	}
	
	if ((copy->params[1] = malloc(sizeof(tSymbol))) == NULL)
	{
		InternalError("Allocation error\n");
		return 1;
	}
	
	if ((copy->params[2] = malloc(sizeof(tSymbol))) == NULL)
	{
		InternalError("Allocation error\n");
		return 1;
	}
				
	copy->funcParams[0] = 's';
	copy->funcParams[1] = 'i';
	copy->funcParams[2] = 'i';
		
	copy->funcTab = initTable(HSIZE);
	
	if ((copy->instructions = malloc(sizeof(tListOfInstrFun))) == NULL)
	{
		InternalError("Allocation error\n");
		return 1;
	}
	
	listInitFun(copy->instructions);
	
	//FIND
	
	tSymbol *find;
	
	if ((find = h_Insert(globTable,"find")) == NULL)
		return 1;
	
	find->type = 'i';
	find->funcOrVar = false;
	find->defined = true;
	
	if ((find->funcParams = malloc(2*sizeof(char))) == NULL)
	{
		InternalError("Allocation error\n");
		return 1;
	}
	
	if ((find->params[0] = malloc(sizeof(tSymbol))) == NULL)
	{
		InternalError("Allocation error\n");
		return 1;
	}
	
	if ((find->params[1] = malloc(sizeof(tSymbol))) == NULL)
	{
		InternalError("Allocation error\n");
		return 1;
	}
					
	find->funcParams[0] = 's';
	find->funcParams[1] = 's';
		
	find->funcTab = initTable(HSIZE);
	
	if ((find->instructions = malloc(sizeof(tListOfInstrFun))) == NULL)
	{
		InternalError("Allocation error\n");
		return 1;
	}
	
	listInitFun(find->instructions);
	
	// SORT	
	
	tSymbol *sort;
	
	if ((sort = h_Insert(globTable,"sort")) == NULL)
		return 1;
	
	sort->type = 's';
	sort->funcOrVar = false;
	sort->defined = true;
	
	if ((sort->funcParams = malloc(sizeof(char))) == NULL)
	{
		InternalError("Allocation error\n");
		return 1;
	}
	
	if ((sort->params[0] = malloc(sizeof(tSymbol))) == NULL)
	{
		InternalError("Allocation error\n");
		return 1;
	}
				
	sort->funcParams[0] = 's';
		
	sort->funcTab = initTable(HSIZE);
	
	if ((sort->instructions = malloc(sizeof(tListOfInstrFun))) == NULL)
	{
		InternalError("Allocation error\n");
		return 1;
	}
	
	listInitFun(sort->instructions);
	
	return 0;
}

int callCopy()
{
	char *par1, *res;
	int par2, par3;
	
	par1 = h_Lookup(globTable,"copy")->params[0]->value.string;
	par2 = h_Lookup(globTable,"copy")->params[1]->value.integer;
	par3 = h_Lookup(globTable,"copy")->params[2]->value.integer;
	
	res = copy(par1,par2,par3);
		
	h_Lookup(globTable,"copy")->value.string = res;
	
	texpVal *tmp2 = malloc(sizeof(texpVal));
	tmp2->typ = 's';
	tmp2->varOrCon = true;
	tmp2->val.string = res;
	pushValueI(tmp2); 
	return 0;
}	

int callLength()
{
	char *str;
	
	str = h_Lookup(globTable,"length")->params[0]->value.string;
	
	int tmp = length(str);
	
	h_Lookup(globTable,"length")->value.integer = tmp;
	
	texpVal *tmp2 = malloc(sizeof(texpVal));
	tmp2->typ = 'i';
	tmp2->varOrCon = true;
	tmp2->val.integer = tmp;
	pushValueI(tmp2); 
	return 0;
}	

int callFind()
{
	char *par1, *par2;
	int res;
	
	par1 = h_Lookup(globTable,"find")->params[0]->value.string;
	par2 = h_Lookup(globTable,"find")->params[1]->value.string;
	
	res = find(par1,par2);
		
	h_Lookup(globTable,"find")->value.integer = res;
	
	texpVal *tmp2 = malloc(sizeof(texpVal));
	tmp2->typ = 'i';
	tmp2->varOrCon = true;
	tmp2->val.integer = res;
	pushValueI(tmp2); 
	return 0;
}	

int callSort()
{
	char *par;
	
	par = h_Lookup(globTable,"sort")->params[0]->value.string;
	
	char *res = sort(par);
	h_Lookup(globTable,"sort")->value.string = res;
	
	texpVal *tmp2 = malloc(sizeof(texpVal));
	tmp2->typ = 's';
	tmp2->varOrCon = true;
	tmp2->val.string = res;
	pushValueI(tmp2); 
	return 0;
}	
