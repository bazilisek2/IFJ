#ifndef STACK_H
#define STACK_H


#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<stdbool.h>
#include "str.h"

#define TOKEN_MAX 150
#define NONTERMINAL 151
#define DOLLAR 149  
#define IDENT 140                  

typedef struct tStack_element{                         
	int symbol;
	int type;
	string value;
	struct tStack_element* next;
} tStack_element;

typedef struct tStack{
	tStack_element* top;
} tStack;

void init_stack(tStack *S);
void push_stack(tStack *S, int symbol, int datatype, string* value);
void pop_stack(tStack *S);
tStack_element* top_stack(tStack *S);
int isterm_stack(int symbol);
tStack_element* topterm_stack(tStack *S);
void insertbeforetopterm_stack(tStack *S, int symbol, int datatype, string* value);
void free_stack(tStack *S);
bool empty_stack(tStack* S);

#endif