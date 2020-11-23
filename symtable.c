#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "str.h"
#include "symtable.h"
#include "global.h"

void tableInit(tSymbolTable *T){
// funkce inicializuje tabulku symbolu

    T->root = NULL;
    T->next = NULL;
    T->previous = NULL;
}

int InsertNode(tSymbolTable * T, tNode * active, string * key){
        
        
        if (active == NULL){

            tNode *node;
            node = malloc(sizeof(struct tNode));
            strInit(&(node->key));
            strCopyString(&(node->key), key);
            node->lptr = NULL;
            node->rptr = NULL;
            node->defined = EMPTY;
            node->data.function = EMPTY;
            node->data.type = EMPTY;
            node->data.param_num = 0;
            node->data.return_num = 0;
            node->data.p_first = NULL;
            node->data.r_first = NULL;

            T->root = node;
            return TRUE;
        }

        else {

            if(strCmpString(key, &active->key) < 0){
            
            if(active->lptr == NULL){
                tNode *node;
                node = malloc(sizeof(struct tNode));
                
                strInit(&(node->key));
                strCopyString(&(node->key), key);
                node->lptr = NULL;
                node->rptr = NULL;
                node->defined = EMPTY;
                node->data.function = EMPTY;
                node->data.type = EMPTY;
                node->data.param_num = 0;
                node->data.return_num = 0;
                node->data.p_first = NULL;
                node->data.r_first = NULL;
                 
                active->lptr = node;
                
                return TRUE;
            }

            else{

                return InsertNode(T, active->lptr, key);
            }
        }
        
        else if (strCmpString(key, &active->key) > 0){
            if (active->rptr == NULL){
                tNode *node;
                node = malloc(sizeof(struct tNode));
                
                strInit(&(node->key));
                strCopyString(&(node->key), key);
                node->lptr = NULL;
                node->rptr = NULL;
                node->defined = EMPTY;
                node->data.function = EMPTY;
                node->data.type = EMPTY;
                node->data.param_num = 0;
                node->data.return_num = 0;
                node->data.p_first = NULL;
                node->data.r_first = NULL;
                
                
                active->rptr = node;
                
                return TRUE;
            }
            else{
                
                return InsertNode(T, active->rptr, key);
            }
        }
        else{

            //item sa nasiel
            return FALSE;
        }
    }

}


tNode * tableSearch(tSymbolTable *T, tNode *active, string *key)
// pokud se dana polozka s klicem key v tabulce symbolu nachazi,
// funkce vrati ukazatel na data teto polozky, jinak vrati NULL
{
    
    if(active == NULL){
        return NULL;
    }
    
    else{
        
        if(strCmpString(key, &active->key) < 0){
            return tableSearch(T, active->lptr, key);
        }
        else if(strCmpString(key, &active->key) > 0){
            return tableSearch(T, active->rptr, key);
        }
        else {    // strcmp vraci 0, nasel item se spravnym klicem a funkce vraci jeho data
            return active;
        }
    }
}

int is_defined(tSymbolTable *T, tNode *active, string *key){
    tNode * var = tableSearch(T, active, key);
    if ((var->defined) == TRUE){
        return 1;
    }
    return 0;
}


tParams * add_param(tSymbolTable *T, tNode *active, string *key){
    tNode * var = tableSearch(T, active, key);
    if (var->data.p_first == NULL){ //prvy parameter
        tParams * param = malloc(sizeof(struct tParams));
        if (param == NULL){
            return NULL;
        }

        else {
            //param->name = EMPTY;
            param->order = 1;
            param->type = EMPTY;
            param->next = NULL;
            var->data.p_first = param;
            var->data.param_num++;
            return param;
        }
    }

    //nejaky tam uz je
    else if (var->data.p_first != NULL){
      tParams * temp = var->data.p_first;
      tParams * previous = NULL;
      int order = 1;
      int param_count = 1;
      while (temp != NULL){
          previous = temp;
          temp = temp->next;
          order ++;
          param_count++;

      }
      temp = malloc(sizeof(struct tParams));
        if (temp == NULL){
            return NULL;
        }
        else {
            previous->next = temp; //napojenie
            //temp->name = EMPTY;
            temp->order = order;
            temp->type = EMPTY;
            temp->next = NULL;
            var->data.param_num++;
            return temp;

        }
 
    }

    return NULL;     
}

tParams * get_param(tSymbolTable *T, tNode *active, string *key, int param_number){
    tNode * var = tableSearch(T, active, key);
    if (var->data.param_num < param_number){
        DEBUG_MODE("Tolko argumentov funkcia nema")
        return NULL;
    }
    else {
        tParams * temp = var->data.p_first;
        int order = 1;
        while (order != param_number){
            temp = temp->next;
            order++;
        }

        return temp;
    }

    return NULL;
}

tReturn * add_return(tSymbolTable *T, tNode *active, string *key){
    tNode * var = tableSearch(T, active, key);
    if (var->data.r_first == NULL){ //prvy return
        tReturn * ret = malloc(sizeof(struct tReturn));
        if (ret == NULL){
            return NULL;
        }

        else {
            ret->order = 1;
            ret->type = EMPTY;
            ret->next = NULL;
            var->data.r_first = ret;
            var->data.return_num++;
            return ret;
        }
    }

    //nejaky tam uz je
    else if (var->data.r_first != NULL){
      tReturn * temp = var->data.r_first;
      tReturn * previous = NULL;
      int order = 1;
      int return_count = 1;
      while (temp != NULL){
          previous = temp;
          temp = temp->next;
          order ++;
          return_count++;

      }
      temp = malloc(sizeof(struct tReturn));
        if (temp == NULL){
            return NULL;
        }
        else {
            previous->next = temp; //napojenie
            temp->order = order;
            temp->type = EMPTY;
            temp->next = NULL;
            var->data.return_num++;
            return temp;

        }
 
    }

    return NULL;     
}


tReturn * get_return(tSymbolTable *T, tNode *active, string *key, int return_number)
{
    tNode * var = tableSearch(T, active, key);
    if (var->data.return_num < return_number){
        DEBUG_MODE("Tolko argumentov funkcia nema")
        return NULL;
    }
    else {
        tReturn * temp = var->data.r_first;
        int order = 1;
        while (order != return_number){
            temp = temp->next;
            order++;
        }
        return temp;
    }
    return NULL;
}

void tableDelete (tSymbolTable *T,tNode *active)
{
	if (T == NULL) 
		return;
	if(active == NULL)
	 	return;

	if (active != NULL) 
	{
		tableDelete(T, active->lptr);
		tableDelete(T, active->rptr);
		free(active);
	}
	T->root = NULL;
	T->next = NULL;
	T->previous = NULL;
}



//initne "zasobnik" tabuliek
void tablelist_init(tTableList *TL){
    TL->Last = NULL;
}


//prida tabulku na koniec zasobnika
void tablelist_add(tTableList *TL, tSymbolTable *new){
    if (TL->Last == NULL){
        TL->Last = new;
        TL->Global = new;
    }

    else { //v liste tabuliek uz nieco je
        new->previous = TL->Last;
        TL->Last = new;
    }
}


//popne a znici tabulku z konca zasobnika
void tablelist_pop(tTableList *TL){
        tSymbolTable * temp = TL->Last;
        TL->Last = TL->Last->previous;
        tableDelete (temp, temp->root);
        
}


//int InsertNode(tSymbolTable * T, tNode * active, string * key)


//TRUE - podarilo sa insertnut
//FALSE - uz tam nieco bolo
//insertne tNode do 
int tablelist_insert(tTableList *TL, string * key){
    tSymbolTable * toInsert = TL->Last;
    int returnval = InsertNode(toInsert, toInsert->root, key);
    if (returnval == TRUE){
        return TRUE;
    }
    else {
        return FALSE;
    }
}


//tNode * tableSearch(tSymbolTable *T, tNode *active, string *key)


tNode * tablelist_search(tTableList *TL, string * key){
    tSymbolTable * Search = TL->Last;
    tNode * found = tableSearch(Search, Search->root, key);
    while (found == NULL){ //v prvej tabulke sa nič nenašlo
        Search = Search->previous;
        if (Search == NULL) { //na konci zásobníka tabuliek
            break;
        }
        found = tableSearch(Search, Search->root, key);
    }

    return found;
}

void deflist_init(tDefList * deflist){
    deflist->first = NULL;
}

int deflist_add(tDefList * deflist, int token){
    tDef * new;
    int count = 1;
    if (deflist->first == NULL){
        new = malloc(sizeof(tDef));
        if (new == NULL){
            return FALSE;
        }
        new->next = NULL;
        new->token = token;
        deflist->first = new;
        deflist->count = 1;

    }
    else { //
        count = 2;
        tDef * active = deflist->first;
        while(active->next != NULL){
            active = active->next;
            count++;
        }
        active->next = malloc(sizeof(tDef));
        if (active->next == NULL){
            return FALSE;
        }
        active->next->next = NULL;
        active->next->token = token; 
        deflist->count = count;
    

    }

    return TRUE;
}

void deflist_delete(tDefList * deflist){
    tDef * active = deflist->first;
    tDef * temp;
    while (active != NULL){
        temp = active;
        active = active->next;
        free(temp);
    }
    deflist->first = NULL;
}

tDef * deflist_get(tDefList * deflist, int order){
    tDef * active;
    if (deflist->first == NULL){
        return NULL;
    }

    int counter = 1;
    active = deflist->first;
    while (counter != order){
        if (active->next == NULL){ //order je vyssie ako realny pocet prvkov v liste
            return NULL;
        }
        active = active->next;
        counter++;
    }

    return active;
}

