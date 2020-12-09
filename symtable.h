 /** IFJ Projekt 2020
 * 
 * Autori:
 * xjorio00 - Vanessa Jóriová 
 * 
 * Súhrn: Štruktúry využivané pri práci s tabuľkou symbolov a v parseri
 * 
 * 
 * */
#ifndef symtable_h
#define symtable_h

#include <stdio.h>
#include <stdbool.h>
#include "str.h"


//struktura uchovavajuca info o parametroch funkcie
typedef struct tParams{
	string name; //nazov
	int type; //typ 
	int order; //poradie
	struct tParams * next;
}tParams;


//struktura uchovavajuca info o navratovych hodnotach funkcie
typedef struct tReturn{
	int type; //typ
	int order; //poradie
	struct tReturn* next;
}tReturn;


//data o danom uzle binarneho stromu
typedef struct tData{
    int type; //typ
    int function; // funkcia?
    int param_num; //pocet parametrov
    int return_num; //pocet navratovych hodnot
    tParams * p_first; //ukazatel na prvy parameter
    tReturn * r_first; //ukazatel na prvu return hodnotu
}tData;


//uzol binarneho stromu
typedef struct tNode{
	string key; //kluc - nazov premennej/funkcie
	string code_gen_key; //nazov modifikovany na generovanie kodu
	tData data;
	int defined; //uz definovana - vyuzite pri funkciach
	struct tNode * lptr;
	struct tNode *rptr;

}tNode;


//tabulka symbolov
typedef struct tSymbolTable
{
	struct tNode * root;
	struct tSymbolTable *next;
	struct tSymbolTable *previous;
	string key;
}tSymbolTable;

//zasobnik tabuliek symbolov
typedef struct tTableList{
	tSymbolTable * Last; 
	tSymbolTable * Global;
}tTableList;


typedef struct tDef{
	int token;
	struct tDef * next;
	string name;
}tDef;


typedef struct tDefList{
	struct tDef * first;
	int count;
}tDefList;

//funkcie pre pracu s tabulkou symbolov
void tableInit(tSymbolTable *T);
int InsertNode(tSymbolTable * T, tNode * active, string * key);
tNode *tableSearch(tSymbolTable *T, tNode *active, string *key);
int is_defined(tSymbolTable *T, tNode *active, string *key);
tParams * add_param(tSymbolTable *T, tNode *active, string *key);
tParams * get_param(tSymbolTable *T, tNode *active, string *key, int param_number);
tReturn * add_return(tSymbolTable *T, tNode *active, string *key);
tReturn * get_return(tSymbolTable *T, tNode *active, string *key, int return_number);
void tableDelete (tSymbolTable *T,tNode *active);
int functionsDeclared(tSymbolTable *T, tNode *active);

//funkcie pre pracu so zasobnikom tabuliek
void tablelist_init(tTableList *TL);
void tablelist_add(tTableList *TL, tSymbolTable *new);
void tablelist_pop(tTableList *TL);
int tablelist_insert(tTableList *TL, string * key);
tNode * tablelist_search(tTableList *TL, string * key);
void tablelist_free (tTableList *TL);

//linked list, vyuzivany v parseri (napr. pri precitani id, id, id... = expr, expr)
//uchovava info o precitanych identifikatoroch a ich typoch
void deflist_init(tDefList * deflist);
int deflist_add(tDefList * deflist, int token, string * atr);
void deflist_delete(tDefList * deflist);
tDef * deflist_get(tDefList * deflist, int order);




#endif
