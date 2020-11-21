/*
 *
 *  Implementace prekladace imperativniho jazyka IFJcode18
 *
 *
 *
 *  symtable.h
 *
 *
 *  xvacul30 - Jan Vaculik
 *
 *  xmensi13 - Jan Mensik
 *
 *
 *
 */

#ifndef symtable_h
#define symtable_h

#include <stdio.h>
#include <stdbool.h>
#include "str.h"

typedef struct tParams{
	string * name;
	int type;
	int order;
	struct tParams * next;
}tParams;

typedef struct tReturn{
	string * name;
	int type;
	int order;
	struct tReturn* next;
}tReturn;

typedef struct tData{
    int type;
    int function; // TRUE/FALSE
    int param_num;
    int return_num;
    tParams * p_first;
    tReturn * r_first;
}tData;


typedef struct tNode{
	string key;
	tData data;
	int defined;
	struct tNode * lptr;
	struct tNode *rptr;

}tNode;

typedef struct tSymbolTable
{
	struct tNode * root;
	struct tSymbolTable *next;
	struct tSymbolTable *previous;
	string key;
}tSymbolTable;


void tableInit(tSymbolTable *T);
int InsertNode(tSymbolTable * T, tNode * active, string * key);
tNode *tableSearch(tSymbolTable *T, tNode *active, string *key);
int is_defined(tSymbolTable *T, tNode *active, string *key);
tParams * add_param(tSymbolTable *T, tNode *active, string *key);
tParams * get_param(tSymbolTable *T, tNode *active, string *key, int param_number);

#endif /* symtable_h */
