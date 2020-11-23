#ifndef symtable_h
#define symtable_h

#include <stdio.h>
#include <stdbool.h>
#include "str.h"

typedef struct tParams{
	string name;
	int type;
	int order;
	struct tParams * next;
}tParams;

typedef struct tReturn{
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

typedef struct tTableList{
	tSymbolTable * Last; 
	tSymbolTable * Global;
}tTableList;


typedef struct tDef{
	int token;
	struct tDef * next;
}tDef;


typedef struct tDefList{
	struct tDef * first;
	int count;
}tDefList;


void tableInit(tSymbolTable *T);
int InsertNode(tSymbolTable * T, tNode * active, string * key);
tNode *tableSearch(tSymbolTable *T, tNode *active, string *key);
int is_defined(tSymbolTable *T, tNode *active, string *key);
tParams * add_param(tSymbolTable *T, tNode *active, string *key);
tParams * get_param(tSymbolTable *T, tNode *active, string *key, int param_number);
tReturn * add_return(tSymbolTable *T, tNode *active, string *key);
tReturn * get_return(tSymbolTable *T, tNode *active, string *key, int return_number);
void tableDelete (tSymbolTable *T,tNode *active);

//tablelist
void tablelist_init(tTableList *TL);
void tablelist_add(tTableList *TL, tSymbolTable *new);
void tablelist_pop(tTableList *TL);
int tablelist_insert(tTableList *TL, string * key);
tNode * tablelist_search(tTableList *TL, string * key);

//deflist
void deflist_init(tDefList * deflist);
int deflist_add(tDefList * deflist, int token);
void deflist_delete(tDefList * deflist);
tDef * deflist_get(tDefList * deflist, int order);




#endif
