/*
 *
 *  Implementace prekladace imperativniho jazyka IFJcode18
 *
 *
 *
 *  symtable.c
 *
 *
 *  xvacul30 - Jan Vaculik
 *
 *  xmensi13 - Jan Mensik
 *
 *
 *
 */

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
            node->data.param_num = EMPTY;
            node->data.return_num = EMPTY;
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
                node->data.param_num = EMPTY;
                node->data.return_num = EMPTY;
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
        return TRUE;
    }
    return FALSE;
}


tParams * add_param(tSymbolTable *T, tNode *active, string *key){
    tNode * var = tableSearch(T, active, key);
    if (var->data.p_first == NULL){ //prvy parameter
        tParams * param = malloc(sizeof(struct tParams));
        if (param == NULL){
            return NULL;
        }

        else {
            param->name = EMPTY;
            param->order = 1;
            param->type = EMPTY;
            param->next = NULL;
            var->data.p_first = param;
            var->data.param_num++;
            return param;
        }
    }

    //nejaky tam už je
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
            temp->name = EMPTY;
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













