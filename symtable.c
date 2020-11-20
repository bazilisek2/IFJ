#include <stdio.h>
#include <stdlib.h>
#include "symtable.h"

void TInit (symtablePtr *RootPtr) {
  // Funkce provede počáteční inicializaci stromu před jeho prvním použitím.
	*RootPtr = NULL;
}

int TSearch (symtablePtr RootPtr, char K, int *Content)	{
/*  
** Funkce vyhledá uzel s klíčem K.
** Pokud je takový nalezen, vrací funkce hodnotu TRUE a v proměnné Content se
** vrací obsah příslušného uzlu. Pokud příslušný uzel není nalezen, vrací funkce
** hodnotu FALSE a obsah proměnné Content není definován 
**/
	if (RootPtr == NULL)
		return FALSE;
	else
	{
		if (RootPtr->Key == K)
		{
			*Content = RootPtr->Data;
			return TRUE;
		}
		else if (RootPtr->Key > K)
			return (TSearch(RootPtr->LPtr, K, Content));
		else 
			return (TSearch(RootPtr->RPtr, K, Content));
	}
}

void TInsert (symtablePtr* RootPtr, char K, int Content)	{
/*   
** Vloží do stromu RootPtr hodnotu Content
** Pokud již uzel se zadaným klíčem ve stromu existuje, bude obsah uzlu
** s klíčem K nahrazen novou hodnotou.
**/
	if ((*RootPtr) == NULL) //vytvorenie noveho uzlu
	{
		(*RootPtr) = malloc(sizeof(struct tBSTNode));
		if ((*RootPtr) != NULL)
		{
			(*RootPtr)->Key = K;
			(*RootPtr)->Data = Content;
			(*RootPtr)->LPtr = NULL;
			(*RootPtr)->RPtr = NULL;
		}
	}
	else
	{
		if ((*RootPtr)->Key == K) //ak najdeme uzol s rovnakym klucom aktualizujeme obsah
			(*RootPtr)->Data = Content;
		else if ((*RootPtr)->Key > K) //ak je hladany kluc vacsi ako aktualny, ideme dolava
			TInsert(&(*RootPtr)->LPtr, K, Content);
		else //ak je hladany kluc mensi ako aktualny, ideme doprava
			TInsert(&(*RootPtr)->RPtr, K, Content);	
	}
}

void TDispose (symtablePtr *RootPtr) {
// Zruší celý binární vyhledávací strom a korektně uvolní paměť.

	if ((*RootPtr) != NULL)
	{
		if ((*RootPtr)->LPtr != NULL)
			TDispose(&(*RootPtr)->LPtr);
		if ((*RootPtr)->RPtr != NULL)
			TDispose(&(*RootPtr)->RPtr);
		free(*RootPtr);
		(*RootPtr) = NULL;
	}
}
