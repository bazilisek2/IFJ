/****
 * IFJ Projekt 2020
 * 
 * Autori:
 * xbella01 - Magdaléna Bellayová 
 * 
 * Súhrn: Práca so zásobníkom 
 * 
 * */

#ifndef STACK_H
#define STACK_H

#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<stdbool.h>
#include "str.h"

#define TOKEN_MAX 150 //maximalna hodnota termov
#define NONTERMINAL 151 //neterminal
#define DOLLAR 149 //dolar
#define IDENT 140 //oznacenie konstanty                  

//struktura elementu zasobnika
typedef struct tStack_element{                         
	int symbol;
	int type;
	string value;
	struct tStack_element* next;
} tStack_element;

//zasobnik
typedef struct tStack{
	tStack_element* top;
} tStack;

void init_stack(tStack *S); //inicializacia stacku
void push_stack(tStack *S, int symbol, int datatype, string* value); //vlozenie na vrchol zasobniku
void pop_stack(tStack *S); //odstranenie vrchneho prvku zo zasobnika 
tStack_element* top_stack(tStack *S); //vracia vrchol zasobnika
int isterm_stack(int symbol); //vracia či je symbol term
tStack_element* topterm_stack(tStack *S); //vracia najvrchnejsi term
void insertbeforetopterm_stack(tStack *S, int symbol, int datatype, string* value); //vlozi prvok za vrchný term
void free_stack(tStack *S); //vyprazdni stack
bool empty_stack(tStack* S); //vrati ci je stack prazdny

#endif
