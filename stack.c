/****
 * IFJ Projekt 2020
 * 
 * Autori:
 * xbella01 - Magdaléna Bellayová 
 * 
 * Súhrn: Práca so zásobníkom  
 * 
 * */


#ifndef stack_h
#define stack_h


#include <stdio.h>
#include "symtable.h"
#include "stack.h"
#include "error.h"

//inicializacia stacku
void init_stack(tStack *S)
{
	S->top = NULL;
	string atr;
	strInit(&atr);
	push_stack(S, DOLLAR, 0, &atr);
}

//vlozenie na vrchol zasobniku
void push_stack(tStack *S, int symbol, int datatype, string* value)
{
	tStack_element* new_item = (tStack_element*)malloc(sizeof(tStack_element));
	
	if (new_item == NULL)
		return;

	new_item->symbol = symbol;
	new_item->type = datatype;
	strInit(&(new_item->value));
	strCopyString(&(new_item->value),value);
	new_item->next = S->top;
	S->top = new_item;
}

//odstranenie vrchneho prvku zo zasobnika
void pop_stack(tStack *S)
{
	if (S->top == NULL)
	{
		return;
	}
	else
	{
		tStack_element* tmp = S->top;
		S->top = tmp->next;
		free(tmp);
	}
}

//vracia vrchol zasobnika
tStack_element* top_stack(tStack *S)
{
	return S->top;
}

//vracia či je symbol term
int isterm_stack(int symbol)
{
	return (symbol < TOKEN_MAX);
}

//vracia najvrchnejsi term
tStack_element* topterm_stack(tStack *S)
{
	tStack_element* tmp = S->top;
	while (tmp != NULL)
	{
		if (isterm_stack(tmp->symbol))
			return tmp;
		else
			tmp = tmp->next;
	}
	return NULL;
}

//vlozi prvok za vrchný term
void insertbeforetopterm_stack(tStack *S, int symbol, int datatype, string* value)
{
	tStack_element  *tmp = S->top;
	tStack_element  *prev_item = NULL;

	while (tmp != NULL)
	{
		if (isterm_stack(tmp->symbol))
		{
			tStack_element *new_item = (tStack_element *)malloc(sizeof(tStack_element ));
			if (new_item == NULL)
				return;

			new_item->symbol = symbol;
			new_item->type = datatype;
			strInit(&(new_item->value));
			strCopyString(&(new_item->value), value);

			if (prev_item == NULL)
			{
				new_item->next = S->top;
				S->top = new_item;
				return;
			}
			else
			{
				new_item->next = prev_item->next;
				prev_item->next = new_item;
				return;
			}
		}
		else
		{
			prev_item = tmp;
			tmp = tmp->next;
		}
	}
}

//vyprazdni stack
void free_stack(tStack *S)
{
	while (S->top)
	{
		pop_stack(S);
	}
}

//vrati ci je stack prazdny
bool empty_stack(tStack* S)
{
	return(S->top==0);
}

#endif
