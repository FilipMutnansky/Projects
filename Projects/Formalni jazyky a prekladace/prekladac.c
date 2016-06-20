 // prekladac.c
 // Implementacia interpretu imperativneho jazyka IFJ14, 15.10.2014
 // Autor: Viktor Toth,xtothv01, FIT
 //        Peter Nagy,xnagyp01, FIT
 //        Dusan Zeliar,xzelia00, FIT
 //        Filip Mutnansky,xmutna00 , FIT
 //        Marek Ochodek,xochod01, FIT
 // Prelozene: gcc 4.8.2
 // .c subor prekladac.c
 // zakladna kostra prekladaca
 

#include <stdio.h>
#include "parsing.h" 
#include "error.h"
#include "interpret.h"


tListOfInstr *list;
TStack_val *stack;
int numOfLabels;
 
FILE * source_file; 
int open_file(const char * filename) {
	 if ((source_file=fopen(filename,"rb"))==NULL){
	    Warning("Subor %s sa nepodarilo otvorit \n",filename);
	    return 1;
     }
     return 0;       
}
 
 int main (int argc, char **argv) 
 {
	if ( (argc!=2 || open_file(argv[1]) )  ){
	   return 99;
	}
	
  // zasobnik tabuliek symbolov
	if ((s = malloc(sizeof(struct Stack_tab))) == NULL)
	{
		InternalError("Allocation error\n");
		return false;
	}

	Stack_init(s);
  
  // instrukcna paska 
	if ((list = malloc(sizeof(tListOfInstr))) == NULL)
	{
		InternalError("Allocation error\n");
		return false;
	}
   
	listInit(list);
   
   // zasobnik hodnot vo vyraze
	if ((stack = malloc(sizeof(TStack_val))) == NULL)
	{
	   InternalError("Allocation error\n");
	   return false;
	}   
   
	Stack2_init(stack);
	
	// zavolanie syntaktickej analyzy
	prog();
	
	// zavolanie interpretu
	if ( (listGetPointerLast(list)) && (errflag == false) ){
	     interpret(list); 
	}
    
    // uvolnenie zdorojov 
	clearTable(globTable);
	listFree(list);

	return errnumber;
 }
	  
 
