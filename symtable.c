/****
 * IFJ Projekt 2020
 * 
 * Autori:
 * xjorio00 - Vanessa Jóriová 
 * 
 * Súhrn: Štruktúry využivané pri práci s tabuľkou symbolov a v parseri
 * 
 * 
 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "str.h"
#include "symtable.h"
#include "global.h"


//inicializacia tabulky symbolov
void tableInit(tSymbolTable *T){

    T->root = NULL;
    T->next = NULL;
    T->previous = NULL;
}

//pridanie uzlu do tabulky symbolov
int InsertNode(tSymbolTable * T, tNode * active, string * key){
        
        
    
        if (active == NULL){

            tNode *node;
            node = malloc(sizeof(struct tNode));
            strInit(&(node->key));
            strInit(&(node->code_gen_key));
            strCopyString(&(node->key), key);
            strCopyString(&(node->code_gen_key), &(node->key));
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
                strInit(&(node->code_gen_key));
                strCopyString(&(node->key), key);
                strCopyString(&(node->code_gen_key), &(node->key));
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
                strInit(&(node->code_gen_key));
                strInit(&(node->key));
                strCopyString(&(node->key), key);
                strCopyString(&(node->code_gen_key), &(node->key));
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



//vyhladanie kluca v tabulke symbolov
tNode * tableSearch(tSymbolTable *T, tNode *active, string *key)
{
    
    if (active == NULL){
        return NULL;
    }
    
    else{
        
        if (strCmpString(key, &active->key) < 0){
            return tableSearch(T, active->lptr, key);
        }
        else if(strCmpString(key, &active->key) > 0){
            return tableSearch(T, active->rptr, key);
        }
        else {    
            return active;
        }
    }
}


//vracia, ci je uzol definovany
int is_defined(tSymbolTable *T, tNode *active, string *key){
    tNode * var = tableSearch(T, active, key);
    if ((var->defined) == TRUE){
        return 1;
    }
    return 0;
}


//prida parameter
tParams * add_param(tSymbolTable *T, tNode *active, string *key){
    tNode * var = tableSearch(T, active, key);
    if (var->data.p_first == NULL){ //prvy parameter
        tParams * param = malloc(sizeof(struct tParams));
        if (param == NULL){
            return NULL;
        }

        else {
            strInit(&param->name);
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
            strInit(&temp->name);
            temp->order = order;
            temp->type = EMPTY;
            temp->next = NULL;
            var->data.param_num++;
            return temp;

        }
 
    }

    return NULL;     
}

//ziska n-ty parameter
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



//ziska n-tu navratovu hodnotu
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


//vymaze tabulku symbolov
void tableDelete (tSymbolTable *T,tNode *active)
{
    if (T == NULL){
        return;
    } 
    if (active == NULL){
      return;  
    } 
    
    while (active != NULL)
    {
        if (active->lptr != NULL){
            tableDelete(T, active->lptr);
            active->lptr = NULL;
        }
        else if (active->rptr != NULL){
            tableDelete(T, active->rptr);
            active->rptr = NULL;
        }
        else{
            strFree(&active->key);
            if (T->root->lptr == NULL && T->root->rptr == NULL){
                T->root = NULL;
            }

            while(active->data.p_first != NULL){
                tParams * tempparam = active->data.p_first;
                active->data.p_first = tempparam->next;
                strFree(&tempparam->name);
                free(tempparam);
            }

            while(active->data.r_first != NULL){
                tReturn * tempretunr = active->data.r_first;
                active->data.r_first = tempretunr->next;
                free(tempretunr);
            }


            
            free (active);
            return;
        }
    }
}


//zisti, ci su vsetky funkcie definovane
int functionsDeclared(tSymbolTable *T, tNode *active){
    int result;
    if(active->lptr != NULL){

        result = functionsDeclared(T, active->lptr);
        if (result == FALSE) {
            return FALSE;
        }
    }
    
    if(active->rptr != NULL){

        result = functionsDeclared(T, active->rptr);
        if (result == FALSE) {
            return FALSE;
        }
    }
    
    if (active->data.function == TRUE){
        if (active->defined == FALSE){
          return FALSE;  
        } 
    }
    return TRUE;
    
}

//initne "zasobnik" tabuliek
void tablelist_init(tTableList *TL){
    TL->Last = NULL;
}


//prida tabulku na koniec zasobnika
void tablelist_add(tTableList *TL, tSymbolTable *new){
    if (TL->Last == NULL){
        TL->Last = new;
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

//TRUE - podarilo sa insertnut
//FALSE - uz tam nieco bolo
//insertne tNode do zasobnika tabuliek
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

//vrati prvy najdeny node zo zasobnika tabuliek
tNode * tablelist_search(tTableList *TL, string * key){
    tSymbolTable * Search = TL->Last;
    tNode * found = tableSearch(Search, Search->root, key);
    if (found != NULL){
        //printf("Nieco sa naslo hned na prvy krat\n");
    }
    while (found == NULL){ //v prvej tabulke sa nič nenašlo
        Search = Search->previous;
        if (Search == NULL) { //na konci zásobníka tabuliek
            break;
        }
        found = tableSearch(Search, Search->root, key);
        if (found != NULL){ 
            break;
        }

    }

    return found;
}

//uvolni zasobnik tabuliek
void tablelist_free (tTableList *TL){
    while(TL->Last != NULL) {
        tSymbolTable *temp;
        temp = TL->Last;
        TL->Last = temp->previous;
        tableDelete(temp, temp->root);
        free(temp);
    }
    TL->Last = NULL;
    
}

//vytvori linked list id-ciek
void deflist_init(tDefList * deflist){
    deflist->first = NULL;
}

//prida polozku do deflistu
int deflist_add(tDefList * deflist, int token, string * atr){
    tDef * new;
    int count = 1;
    if (deflist->first == NULL){
        new = malloc(sizeof(tDef));
        if (new == NULL){
            printf("Error");
        }
        strInit(&new->name);
        new->next = NULL;
        new->token = token;
        deflist->first = new;
        deflist->count = 1;
        strCopyString(&(new->name), atr);
        

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
        strInit(&active->next->name);
        active->next->next = NULL;
        active->next->token = token; 
        deflist->count = count;
        strCopyString(&(active->next->name), atr);
    

    }

    return TRUE;
}


//vymaze deflist
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


//ziska n-tu polozku z deflistu
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

