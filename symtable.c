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

void ReplaceByRightmost (symtablePtr PtrReplaced, symtablePtr*RootPtr) {
/*   
** Pomocná funkce pro vyhledání, přesun a uvolnění nejpravějšího uzlu.
**/
	if ((*RootPtr) != NULL) //hodnota ukazatela nebude NULL
	{
		if((*RootPtr)->RPtr != NULL) //ak neni najpravejsi uzol
			ReplaceByRightmost(PtrReplaced, &(*RootPtr)->RPtr);
		else
		{
			PtrReplaced->Data = (*RootPtr)->Data;
			PtrReplaced->Key = (*RootPtr)->Key;
		}
	}
}

void TDelete (symtablePtr *RootPtr, char K) 		{
/*   
** Zruší uzel stromu, který obsahuje klíč K.
**/
	symtablePtr item = NULL;
	if (((*RootPtr)->Key > K) && ((*RootPtr)->LPtr != NULL)) //ide dolava
		TDelete(&(*RootPtr)->LPtr, K);
	else if (((*RootPtr)->Key < K) && ((*RootPtr)->RPtr != NULL)) //ide doprava
		TDelete(&(*RootPtr)->RPtr, K);
	else if ((*RootPtr)->Key == K) //nachadzame sa na prvku ktory chceme odstranit
	{
		if (((*RootPtr)->RPtr == NULL) && ((*RootPtr)->LPtr == NULL)) //nema vpravo ani vlavo ziadny dalsi uzol
		{
		 	free(*RootPtr);
			(*RootPtr) = NULL;
		}
		else if (((*RootPtr)->RPtr == NULL) && ((*RootPtr)->LPtr != NULL)) //ma dalsi uzol len vpravo

		{
			item = (*RootPtr);
			(*RootPtr) = (*RootPtr)->LPtr;
			free(item);
			item = NULL;
		}
		else if (((*RootPtr)->RPtr != NULL) && ((*RootPtr)->LPtr == NULL)) //ma dalsi uzol len vlavo
		{
			item = (*RootPtr);
			(*RootPtr) = (*RootPtr)->RPtr;
			free(item);
			item = NULL;
		}
		else
		{
			symtablePtr item1 = NULL;
			item1 = (*RootPtr);
			ReplaceByRightmost(item1, &(*RootPtr)->LPtr);
			TDelete(&(*RootPtr)->LPtr, item1->Key);
		}
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
