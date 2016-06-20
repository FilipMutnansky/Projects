// sem.c
 // Implementacia interpretu imperativneho jazyka IFJ14, 15.10.2014
 // Autor: Viktor Toth,xtothv01, FIT
 //        Peter Nagy,xnagyp01, FIT
 //        Dusan Zeliar,xzelia00, FIT
 //        Filip Mutnansky,xmutna00 , FIT
 //        Marek Ochodek,xochod01, FIT
 // Prelozene: gcc 4.8.2
 // .c subor sem.c
 // semanticke funkcie
 
#include "sem.h"
#include "ial.h"
#include "error.h"
#include "parsing.h"

tToken act_token;

void Stack_init(TStack_tab *s) 
{
  s->STop = NULL;	
}

int Stack_push(Thtab_t *tab)
{
	if (tab == NULL)
		return 1;
	
  	tab->next = s->STop;
  	s->STop = tab;	
  	return 0;
}

void Stack_pop()
{
	if (s->STop)
	{	
		Thtab_t *tmp = s->STop;
		s->STop = s->STop->next;
		free(tmp);
	}	
}

void Stack2_init(TStack_val *s) 
{
  s->STop = NULL;	
}

// vlozi hodnotu z vyrazu na zasobnik
int Stack2_push(TStack_val *s, texpVal *value)
{
	texpVal *t;
	
	if ((t = malloc(sizeof(texpVal))) == NULL)
	{
		InternalError("Alloctaion error\n");
		return 1;
	}
	
	if (value->varOrCon == false)
	{
		t->id = value->id;
		t->varOrCon = false;
		t->typ = value->typ;
		
		if (findSymbolInFun(t->id)->state == false)
		{
			Uninitialized();
			return 1;
		}
						
	} else if (value->varOrCon == true)
		{
			t->typ = value->typ;
			if (value->typ == 'i')
			{
				t->val.integer = value->val.integer;
				t->varOrCon = true;
			}
				if (value->typ == 'r')
				{
					t->val.real = value->val.real;
					t->varOrCon = true;
				}
					if (value->typ == 's')
					{
						t->val.string = value->val.string;
						t->varOrCon = true;
					}
						if (value->typ == 'b')
						{
							t->val.boolean = value->val.boolean;
							t->varOrCon = true;
						}
		}
		
  	t->next = s->STop;
  	s->STop = t;	
  	return 0;
}

void Stack2_pop(TStack_val *s)
{
	if (s->STop) {
		texpVal *tmp = s->STop;
		s->STop = s->STop->next;	
		free(tmp);		
	}
}

tSymbol *lookInTables(const char *key)
{
	tSymbol *symb = NULL;

	if (s->STop && (symb = h_Lookup(s->STop,key)))
	{
		return symb;
	}
		else
			return h_Lookup(globTable,key);
}

tSymbol *lookInTablesSem(const char *key)
{
	tSymbol *symb = NULL;
		
	if (h_Lookup(globTable,actFunction)->funcTab && (symb = h_Lookup(h_Lookup(globTable,actFunction)->funcTab,key)))
		return symb;
		else
			return h_Lookup(globTable,key);
}

char *addCharToString(char *s, char c)
{
	char *s2 = NULL;
	int len = 0;
	
	if (s == NULL) 
	{
		len = 0;
		if ((s2 = malloc(len+2)) == NULL)
			return NULL;
	}
			else 
			{
				len = strlen(s);
				if ((s2 = malloc(len+2)) == NULL)
					return NULL;
				strcpy(s2,s);
			}
				
	s2[len] = c;
	s2[len+1] = '\0';
	
	if (s) 
		free(s);
	
	return s2;
}

void listInit(tListOfInstr *L)
{
  L->First  = NULL;
  L->Last   = NULL;
  L->Active = NULL;
}

void listFree(tListOfInstr *L)
{
  tListItem *ptr;
  while (L->First != NULL)
  {
    ptr = L->First;
    L->First = L->First->next;
    free(ptr);
  }
}

int listInsertLast(tListOfInstr *L, tInstr I)
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

int listInsertNext(tListOfInstr *L, tInstr I)
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

void generateInstructionNext(tListOfInstr *list, int instType, void *addr1, void *addr2, void *addr3)
{
   tInstr I;
   I.instType = instType;
   I.addr1 = addr1;
   I.addr2 = addr2;
   I.addr3 = addr3;
   listInsertNext(list, I);
}

void listFirst(tListOfInstr *L)
{
  L->Active = L->First;
}

void listLast(tListOfInstr *L)
{
  L->Active = L->Last;
}

void listNext(tListOfInstr *L)
{
  if (L->Active != NULL)
  L->Active = L->Active->next;
}

void listPrevious(tListOfInstr *L)
{
  if (L->Active != NULL)
  L->Active = L->Active->prev;
}

void *listGetPointerLast(tListOfInstr *L)
{
  return (void*) L->Last;
}

tInstr *listGetData(tListOfInstr *L)
{
  return &(L->Active->Instr);
}

// vygeneruje instrukciu a prida ju do zoznamu instrukcii
void generateInstruction(tListOfInstr *list, int instType, void *addr1, void *addr2, void *addr3)
{
   tInstr I;
   I.instType = instType;
   I.addr1 = addr1;
   I.addr2 = addr2;
   I.addr3 = addr3;
   listInsertLast(list, I);
}

/* FUNCKIA PRE KONTROLU SEMANTICKEHO VYZNAMU*/
bool semCheckVariable(){
	char *id = act_token.attribute.string;
	tSymbol *tmp = findSymbolInFun(id);
 	if (tmp == NULL || tmp->funcOrVar == false)
	   return SemError();
	return true;
}
