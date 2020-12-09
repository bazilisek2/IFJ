/****
 * IFJ Projekt 2020
 * 
 * Autori:
 * xjorio00 - Vanessa Jóriová 
 * 
 * Súhrn: ústredný modul celého programu, vykonávajúci syntaktickú a sémantickú analýzu, 
 * vo svojej činnosti využívajú ostatné moduly (scaner a precedenčnú analýzy výrazov) 
 * 
 * 
 * */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "scanner.h"
#include "global.h"
#include "syntax_analysis.h"
#include "symtable.h"
#include "precanal.h"
#include "codegen.h"



#define BOOL 0
#define FUNC_BODY 1
#define FOR_BODY 2


// nacitavanie funkcie - CHECK, pokial uz je v tabulke symbolov a kontrolujeme jej parametre atď.,
// a LOAD, pokial sa v nej este nenachadza (volanie funkcie pred jej definicou)
#define LOAD -4
#define CHECK -5
#define PRINT_M -6 //mode na print - pocet parametrov sa nekontroluje 


//globalne premenne, podielajuce sa na generacii unikatnych labelov
int if_label_num = 0; 
int global_scope_num = 0; 
int for_label_num = 0; 
int recursive_counter = 1;

//globalne premenne sledujuce vyskyt mainu, returnu a EOLu napriec funkciami
int EOL_flag = 0;
int main_found = FALSE;
int return_found = FALSE;

//globalne struktury vyuzivane v celom programe
tSymbolTable *GlobalTable;
tTableList * TL;
tDefList * deflist;
string atr;
string saved_atr;
int saved_token = EMPTY;

//prekopirovanie stringu do str structu za pomocu strAddChar
void copystr(char str[], string * atr){ 
    int len = strlen(str);   
    for (int i = 0; i < len; i++){
        strAddChar(atr, str[i]);
    }
}

//je token term
int is_term(int token){ 
    if (token == UNDERLINE || token == IDENTIFIER || token == INT || token == FLOAT_N || token == STRING_LIT || token == FLOAT_L){
        return TRUE;
    }
    
    return FALSE;
}

//typ termu - string, float, lit
int is_type(int token){
    if (token == INT_T || token == FLOAT64 || token == STRING_T){
        return TRUE;
    }
    
    return FALSE;
}

void built_in_insert();
int analysis_start();
int prolog();
int func();
int args(string func_name, int mode);
int return_params(string func_name, int mode);
int s_body(int func_body, string func_name, int for_label);
int expr(int * expr_type);
int f_params(string name, int mode);
int simple_func_or_expr(int for_assign);
int func_or_expr(int for_assign);
int s_if(string funcname);
int s_return(string funcname);
int s_for(string funcname);
int f_definition();
int f_assign();
int program();
int s_body_r(int func_body, string func_name, int for_label);
int s_definition(int func_body, int for_label);



int analysis_start(){
    strInit(&atr);
    strInit(&saved_atr);
    GlobalTable = malloc(sizeof(tSymbolTable));
    tableInit(GlobalTable);
    TL = malloc(sizeof(tTableList));
    tablelist_init(TL);
    tablelist_add(TL, GlobalTable);
    deflist = malloc(sizeof(tDefList));
    deflist_init(deflist);
    int retvalue = EMPTY;

    codegen_start();
    built_in_insert(); //pridanie built-in funkcii do tabulky 

    int token = get_token(PEEK, &EOL_flag, &atr);
    if (token == LEX_ERROR){
        return LEX_ERROR;
    }
    if (token == END_OF_FILE){ 
        DEBUG_MODE("Prazdny subor")   
        return TRUE; //MOZE BYT SUBOR PRAZDNY?
    } 
    if ((retvalue = prolog()) != TRUE){
        DEBUG_MODE("Zly prolog")
        return retvalue;
    }

    
    if ((retvalue = program()) != TRUE){
        DEBUG_MODE("Zla sekvencia funkcii")
        return retvalue;
    }

    if (main_found != TRUE){
        DEBUG_MODE("Chyba main!");
        return SEM_ERROR; 
    }

    if (functionsDeclared(GlobalTable, GlobalTable->root) == FALSE){
        DEBUG_MODE("Nie vsetky funkcie su deklarovane")
        return SEM_ERROR;
    }
    strFree(&atr);
    strFree(&saved_atr);
    tablelist_free(TL);
    free(TL);           
    return TRUE;
}

//postupnost funkcii 
int program(){
    int retvalue = EMPTY;
    int token = get_token(GET, &EOL_flag, &atr);
        if (token == LEX_ERROR){
            return LEX_ERROR;
        }

        if (token == FUNC && EOL_flag == EOL_FOUND){ //pred func, za package main EOL
            if ((retvalue = func()) != TRUE) {
                return retvalue;
            }
            if ((retvalue = program()) != TRUE) {
                return retvalue;
            }
        }
        else if (token == END_OF_FILE){
            DEBUG_MODE("Koniec suboru")
            return TRUE;
        }
        else { //cokolvek, co nezacina tokenom "func"
            DEBUG_MODE("Zla struktura programu - nie je sekvencia funkcii")
            return SYN_ERROR;
        }

    return TRUE;
}



//Uz nacitane: func
int func(){
    int retvalue = EMPTY;
    int in_main = FALSE;
    tNode * funcnode;
    int mode = LOAD; //load - check
    int token;
    string func_name;
    strInit(&func_name);  
    recursive_counter = 1; //vynulovanie pre nove pocitanie argumentov/return hodnot
    //func ID
    token = get_token(GET, &EOL_flag, &atr);
    if (token == LEX_ERROR){
        return LEX_ERROR;
    }
    if (token != IDENTIFIER || EOL_flag != NO_EOL) {
        DEBUG_MODE("Zle ID")
        return SYN_ERROR;
    }
    
    if ((funcnode = tablelist_search(TL, &atr)) != NULL){ 
        if (funcnode->defined == FALSE){
            mode = CHECK;
            DEBUG_MODE("Najdena nedefinovana, zatial nepopisana funkcia")
            funcnode->defined = TRUE;
        }
        else { //je uz definovana
            DEBUG_MODE("Redefinicia funkcie");
            return SEM_ERROR;     
        }
    }

    if (mode == LOAD){
        retvalue = tablelist_insert(TL, &atr);    
        funcnode = tablelist_search(TL, &atr); 
        funcnode->defined = TRUE;
        funcnode->data.function = TRUE;
    }

    //je to main?
    if ((strcmp(atr.str, "main") == 0)) {
        in_main = TRUE;
        codegen_main(); //generacia labelu pre main
        if (main_found == TRUE){
            //redefinícia mainu
            DEBUG_MODE("Redefinícia mainu")
            return SEM_ERROR;
        }
        main_found = TRUE;
    }
    else {
        codegen_funclabel(atr.str); //vygenerovanie ostatnych labelov
    }

    strCopyString(&func_name, &atr); //ulozenie nazvu funkcie, do ktorej tela vchadzame
    //func ID (
    token = get_token(GET, &EOL_flag, &atr);
    if (token == LEX_ERROR){
        return LEX_ERROR;
    }

    if (token != L_BRACKET || EOL_flag != NO_EOL){
        DEBUG_MODE("Chyba vo funkcii v zatvorke pred argumentmi")
        return SYN_ERROR;
    }
    //PEEK
    token = get_token(PEEK, &EOL_flag, &atr);
    switch (token){
        case IDENTIFIER:
            //func ID (string...
            recursive_counter = 1;
            if ((retvalue = args(func_name, mode)) != TRUE){
                DEBUG_MODE("Zle argumenty funkcie!")
                return retvalue;
            }
            break;

        case R_BRACKET:
            token = get_token(GET, &EOL_flag, &atr); //realne nacitanie zatvorky
            if (EOL_flag != NO_EOL){
                return SYN_ERROR;
            }

            if (mode == CHECK){
                funcnode = tablelist_search(TL, &func_name);
                if (funcnode->data.param_num != 0){
                    DEBUG_MODE("Skorej volana funkcia ma zly pocet parametrov")
                    return PARAM_ERROR;
                }
            }
            //func ID ( )
            break;

        default:
            if (token == LEX_ERROR){
                return LEX_ERROR;
            }
            //func ID ( chyba
            DEBUG_MODE("Neocakavany znak v argumentoch")
            return SYN_ERROR;
    }

    //func ID (args)...
    token = get_token(GET, &EOL_flag, &atr);
    if (token == LEX_ERROR){
        return LEX_ERROR;
    }
    if ((token == L_BRACKET1) && (EOL_flag == NO_EOL)){
        //func id () {....

        if (mode == CHECK){
            funcnode = tablelist_search(TL, &func_name);
            if (funcnode->data.return_num != 0){
                DEBUG_MODE("Skorej volana funkcia ma zly pocet parametrov")
                return PARAM_ERROR;
            }
        }

        DEBUG_MODE("Return zatvorka skipnuta")
        if ((retvalue = s_body(TRUE, func_name, 0)) != TRUE){
            DEBUG_MODE("Zle telo funkcie")
            return retvalue;
        }
    }
    else if (token == L_BRACKET && EOL_flag == NO_EOL){
         //func ID (args) (...
         token = get_token(PEEK, &EOL_flag, &atr);
         if (token == LEX_ERROR){
             return LEX_ERROR;
         }

        //func ID (args) ()
        if (token == R_BRACKET && EOL_flag == NO_EOL){
            DEBUG_MODE("Zatvorka hned ukoncena");
            token = get_token(GET, &EOL_flag, &atr); //nacitanie )
            if (mode == CHECK){
                funcnode = tablelist_search(TL, &func_name);
                if (funcnode->data.return_num != 0){
                    DEBUG_MODE("Skorej volana funkcia ma zly pocet parametrov")
                    return PARAM_ERROR;
                }
            }
            //func ID (args) () {
            if ((token = get_token(GET, &EOL_flag, &atr)) == L_BRACKET1){
                if ((retvalue = s_body(TRUE, func_name, 0)) != TRUE){
                    DEBUG_MODE("Zle telo funkcie")
                    return retvalue;
                }
            }
            else {
                DEBUG_MODE("Neocakavany znak miesto {")
            }
             
        }

         //func ID (args) (type...
        else if ((is_type(token)) && EOL_flag == NO_EOL){
            recursive_counter = 1;
            if ((retvalue = return_params(func_name, mode)) != TRUE){
                DEBUG_MODE("Zle return parametre")
                return retvalue;
            }

            //func ID (args) (return_params) {
            if ((token = get_token(GET, &EOL_flag, &atr)) == L_BRACKET1){
                if ((retvalue = s_body(TRUE, func_name, 0)) != TRUE){
                    DEBUG_MODE("Zle telo funkcie")
                    return retvalue;
                }
            }
            else {
                DEBUG_MODE("Neocakavany znak miesto {")
            }
             
        }
        else {
            DEBUG_MODE("Zle pokracovanie return zatvorky")
            return SYN_ERROR;
        }
         
    }

    else{ //ani (, ani {
        DEBUG_MODE("Zly zaciatok tela funkcie");
        return SYN_ERROR;
    }
    
    //celá funkcia ok
    if (in_main == TRUE){

    if (funcnode->data.param_num > 0 || funcnode->data.return_num > 0){
            DEBUG_MODE("Funkcia main ma argumenty/navratove hodnoty")
            return PARAM_ERROR;
        }
    }
    
    if (in_main == TRUE){
        //koniec funkcie main
        codegen_end();
        in_main = FALSE;
    }
    else { //koniec uzivatelskej funkcie
        codegen_func_end();
    }
    strFree(&func_name); 
    return TRUE;
    
}



//uvod do postupnosti statementov
int s_body(int func_body, string func_name, int for_label){
    int retvalue = EMPTY;
    return_found = FALSE;
    tNode * funcnode = NULL;
    tParams * tempparam;
    string name; //nazov parametru funkcie pouzity ako lokalna premenna
    string codegen_name;
    strInit(&codegen_name);
    strInit(&name);
    tSymbolTable * LocalTable = malloc(sizeof(tSymbolTable));
    tableInit(LocalTable);
    tablelist_add(TL, LocalTable);
    global_scope_num++;
    

    if((func_body == TRUE) ){
        funcnode = tablelist_search(TL, &func_name);
        
        if (funcnode != NULL){
            int param_number = funcnode->data.param_num; //pocet parametrov funkcie
            int order = 1; //ktory parameter

            while (param_number != 0){
                tempparam = get_param(GlobalTable, GlobalTable->root, &func_name, order);
                strCopyString(&name, &tempparam->name);
                retvalue = tablelist_insert(TL, &name);
                if (retvalue != TRUE){
                    DEBUG_MODE("Redefinicia parametrov")
                    return SEM_ERROR;
                }

                tNode * param_v = tablelist_search(TL, &name);
                //printf("\n%s\n", param_v->key.str);
                param_v->data.function = FALSE;
                param_v->data.type = tempparam->type; 
                strAddChar(&(param_v->code_gen_key), 'f');
                strCopyString(&(codegen_name), &(param_v->code_gen_key));

                codegen_func_param(order, codegen_name.str);

                param_number--;
                order++;
            } 
        }
        
    }

    if ((retvalue = s_body_r(func_body, func_name, for_label)) != TRUE){
            DEBUG_MODE("Zle telo funkcie")
            return retvalue;
        }

    //nacitana }, ktora ukoncuje kazdu postupnost statementov 

    if (func_body == TRUE){

    funcnode = tablelist_search(TL, &func_name);
    if (funcnode->data.return_num != 0){
        if (return_found == FALSE){
            DEBUG_MODE("Chyba return funkcie")
            return PARAM_ERROR;
        }
    }
    }


    tablelist_pop(TL); //zbavenie sa lokalneho ramca
    strFree(&name);
    free(LocalTable);
    
    return TRUE;
}


//postupnost statementov 
int s_body_r(int func_body, string func_name, int for_label){
    int retvalue = EMPTY;
    tNode * tempnode = NULL;
    int mode = CHECK;
        int token = get_token(GET, &EOL_flag, &atr);
        saved_token = token; //premenna cisto len na zachytenie prveho ID
        strCopyString(&saved_atr, &atr); //dosacne ulozenie nazvu premennej   
        if (token == LEX_ERROR){
            return LEX_ERROR;
        }
        if (token == R_BRACKET1){
            if (EOL_flag != EOL_FOUND){
                    return SYN_ERROR;
                }
            return TRUE;
        }
        
        switch(token){
        
        //id
        case IDENTIFIER:
        case UNDERLINE: 

            if (EOL_flag != EOL_FOUND){
                return SYN_ERROR;
            }

            token = get_token(GET, &EOL_flag, &atr);
            if (token == LEX_ERROR){
                return LEX_ERROR;
            }
            switch (token) {
                
                //id := 
                case DEFINITION:

                    DEBUG_MODE("STATEMENT: DEFINITION")
                    if (EOL_flag != NO_EOL){
                        return SYN_ERROR;
                    }

                    if ((retvalue = s_definition(func_body, for_label)) != TRUE){
                         DEBUG_MODE("Chyba v jednoduchom priradeni alebo funkcii")
                        return retvalue;
                    }
                    break;
                
                //id = jednoduché priradenie alebo priradenie funkcie
                case ASSIGN:

                    DEBUG_MODE("STATEMENT: ASSIGN OR FUNC CALL (one return value)")
                    if (EOL_flag != NO_EOL){
                        return SYN_ERROR;
                    }
                    if ((retvalue = simple_func_or_expr(FALSE)) != TRUE){
                         DEBUG_MODE("Chyba v jednoduchom priradeni alebo funkcii")
                        return retvalue;
                    }
                    break;

                //id(       id savnute v saved_token, atr v tempatr
                case L_BRACKET:
                    DEBUG_MODE("STATEMENT: FUNCTION CALL (without return values)") 
                    if (EOL_flag != NO_EOL){
                        return SYN_ERROR;
                    }
                    // id (
                    //vyhladam predosle ID (uz viem, ze ide o funkciu)
                    tempnode = tablelist_search(TL, &saved_atr);
                    if (tempnode == NULL){
                        mode = LOAD;
                        DEBUG_MODE("Zatial nedefinovane volanie funkcie");
                        retvalue = InsertNode(GlobalTable, GlobalTable->root, &saved_atr);
                        tempnode = tablelist_search(TL, &saved_atr);
                        tempnode->data.function = TRUE;
                        tempnode->defined = FALSE;
                    }

                    if (mode == CHECK){
                        if (tempnode->data.function != TRUE){
                            DEBUG_MODE("Volana funkcia je premenna")
                            return SEM_ERROR; //?
                        }
                        if (tempnode->data.return_num != 0){
                            DEBUG_MODE("Zly pocet navratovych hodnot funkcie");
                            return PARAM_ERROR;
                        }
                    }

                    if ((strcmp(saved_atr.str, "print") == 0)){
                        DEBUG_MODE("Nacital sa print\n");
                        mode = PRINT_M;
                    }
                   

                    if ((retvalue = f_params(saved_atr, mode)) != TRUE){
                        DEBUG_MODE("Zle parametre funkcie za volanim fukncie")
                        return retvalue;
                    }

                    if ((strcmp(saved_atr.str, "print") != 0)){
                        codegen_call(saved_atr.str);
                    }
                    break;

                    //id, 
                    case COMA:
                        DEBUG_MODE("STATEMENT MULTIPLE DEFINITION/FUNC_CALL")
                        if ((retvalue = func_or_expr(FALSE)) != TRUE){
                            DEBUG_MODE("Zle return parametre")
                            return retvalue;
                        }     
                        break;

                    default:
                        DEBUG_MODE("Neocakavany statement po id")
                        return SYN_ERROR;
                }
                break;
            
            //if...
            case IF:
                if (EOL_flag != EOL_FOUND){
                    return SYN_ERROR;
                }
                if ((retvalue = s_if(func_name)) != TRUE){
                    DEBUG_MODE("Chyba v if")
                    return retvalue;
                }
                break;
            
        
            //return...
            case RETURN:
                return_found = TRUE;
                if(EOL_flag != EOL_FOUND){
                    return SYN_ERROR;
                }
                recursive_counter = 1; //vyresetovanie rec_counteru
                if ((retvalue = s_return(func_name)) != TRUE){
                    DEBUG_MODE("Chyba v return")
                    return retvalue;
                }
                break;

            case FOR:
                if (EOL_flag != EOL_FOUND){
                    return SYN_ERROR;
                }
                if ((retvalue = s_for(func_name)) != TRUE){
                    DEBUG_MODE("Chyba vo for")
                    return retvalue;
                }
                break;

            case R_BRACKET1:
                if (EOL_flag != EOL_FOUND){
                    return SYN_ERROR;
                }

                break;
        
            default:
                DEBUG_MODE("Nacitany neznamy zaciatok statementu")
                return SYN_ERROR;
                    

        }

        if ((retvalue = s_body_r(func_body, func_name, for_label)) != TRUE){
            DEBUG_MODE("Zla sekvencia funkcii")
            return retvalue;
    }

    return TRUE;
}



int s_definition(int func_body, int for_label){
    int expr_type = EMPTY;
    int retvalue = EMPTY;
    string codegen_name;
    strInit(&codegen_name);
    if (saved_token == UNDERLINE){ //definicia do _
        DEBUG_MODE("Definovanie do prazdneho id")
        return OTHER_ERROR;
    }
    //id := <expr>

    if ((retvalue = expression(&expr_type, TL)) != TRUE){
        DEBUG_MODE("Chyba v definici")
        return retvalue;
    }
    
    retvalue = tablelist_insert(TL, &saved_atr);
    if (retvalue == FALSE){
        DEBUG_MODE("Pokus o redefiniciu premennej")
        return SEM_ERROR;
    }
    tNode * tempnode = tablelist_search(TL, &saved_atr);
    if (tempnode != NULL){
        tempnode->data.function = FALSE;
        tempnode->data.type = expr_type;
        int first = global_scope_num/100;
        char c = first + '0'; //global scope na char
        strAddChar(&(tempnode->code_gen_key), c);
        int second = global_scope_num/10;
        c = second + '0'; //global scope na char
        strAddChar(&(tempnode->code_gen_key), c);
        int third = global_scope_num%10;
        c = third + '0'; //global scope na char
        strAddChar(&(tempnode->code_gen_key), c);
    }
    strCopyString(&(codegen_name), &(tempnode->code_gen_key));
    if (func_body == FOR_BODY){
        codegen_for_defvar(for_label, codegen_name.str);
    }
    else {
        codegen_declare(codegen_name.str);
    }
    strFree(&codegen_name);
    return TRUE;
}



//for
int s_for(string funcname){
    int expr_type = EMPTY;
    int retvalue = EMPTY;
    int for_label = for_label_num;
    for_label_num++;
    DEBUG_MODE("STATEMENT FOR")
    //for - pozrie sa, co je za tym
    int token = get_token(PEEK, &EOL_flag, &atr);
    if (token == LEX_ERROR){
        return LEX_ERROR;
    }
    tSymbolTable * For_table = malloc(sizeof(tSymbolTable));
    tableInit(For_table);
    tablelist_add(TL, For_table);
    global_scope_num++;
    //vyraz nebude preskoceny
    if (token != UNDERSCORE){

        if((retvalue = f_definition()) != TRUE){
            DEBUG_MODE("Zla definicia vyrazu vo for")
            return retvalue;
        }
    }
    //for <def>;
    token = get_token(GET, &EOL_flag, &atr);
    if (token == LEX_ERROR){
        return LEX_ERROR;
    }
    if (token != UNDERSCORE || EOL_flag != NO_EOL){
        DEBUG_MODE("Chyba bodkociarka za definition vo for")
        return SYN_ERROR;
    }
    //for <def>; <expr> 

    codegen_for_start(for_label);

    if ((retvalue = expression(&expr_type,TL)) != TRUE){
        DEBUG_MODE("Chyba v expr")
        return retvalue;
    }

    if (expr_type != BOOL){
        DEBUG_MODE("Expr vo for nie je typu bool")
        return TYPE_ERROR;
    }
    codedegen_for_expr(for_label);
    token = get_token(GET, &EOL_flag, &atr);
    if (token == LEX_ERROR){
        return LEX_ERROR;
    }

    if (token != UNDERSCORE || EOL_flag != NO_EOL){
        DEBUG_MODE("Chyba bodkociarka za expresion vo for")
        return SYN_ERROR;
    }

    token = get_token(PEEK, &EOL_flag, &atr);
    if (token == LEX_ERROR){
        return LEX_ERROR;
    }
    //vyraz nebude preskoceny
    codegen_for_assign_start(for_label);
    if (token != L_BRACKET1){
        if((retvalue = f_assign()) != TRUE){
            DEBUG_MODE("Zle priradenie vo for")
            return retvalue;
        }
    }
    codegen_for_assign_end(for_label);
    token = get_token(GET, &EOL_flag, &atr);
    if (token == LEX_ERROR){
        return LEX_ERROR;
    }
    if (token != L_BRACKET1 || EOL_flag != NO_EOL){
        DEBUG_MODE("Chyba { vo for")
        return SYN_ERROR;
    }

    codegen_for_cycle(for_label);

    if ((retvalue = s_body(FOR_BODY, funcname, for_label)) != TRUE){ 
        DEBUG_MODE("Zla sekvencia prikazov v tele foru")
        return retvalue;
        }

    codegen_for_check(for_label);
    codegen_for_end(for_label);


        
    
    tablelist_pop(TL); //popnutie table z assignu
    DEBUG_MODE("For presiel")
    return TRUE;
}

//priradenie vo fore
int f_assign(){
    int retvalue = EMPTY;
    int token;
    token = get_token(GET, &EOL_flag, &atr);
    if (token == LEX_ERROR){
        return LEX_ERROR;
    }
    if (((token != IDENTIFIER) && (token != UNDERLINE)) || EOL_flag != NO_EOL){
        return SYN_ERROR;
    }


    saved_token = token;
    strCopyString(&saved_atr, &atr);
    token = get_token(PEEK, &EOL_flag, &atr); //PEEK
    if (token == LEX_ERROR){
        return LEX_ERROR;
    }
    //id assign - jednoduché priradenie
    if (token == ASSIGN){
        token = get_token(GET, &EOL_flag, &atr); //realne nacitanie
        if ((retvalue = simple_func_or_expr(TRUE)) != TRUE){

            DEBUG_MODE("Chyba v jednoduchej exp vo for")
            return retvalue;
        }
    } 

    else { 
        token = get_token(GET, &EOL_flag, &atr);
        if (token == LEX_ERROR){
            return LEX_ERROR;
        }

        if (token != COMA || EOL_flag != NO_EOL){
            DEBUG_MODE("Zly znak za id vo viacnasobnom priradeni vo fore")
            return SYN_ERROR;
        }

        if ((retvalue = func_or_expr(TRUE)) != TRUE){
            DEBUG_MODE("Chyba vo viacnasobnom priradeni vo for")
            return retvalue;
        }
    } 

    return TRUE;
}

//definicia pre for
int f_definition(){
    int expr_type = EMPTY;
    int retvalue = EMPTY;
    int token;
    string id_atr;
    string codegen_name;
    strInit(&codegen_name);
    strInit(&id_atr);
    //id
    token = get_token(GET, &EOL_flag, &atr);
    if (token == LEX_ERROR){
        return LEX_ERROR;
    }

    if ( token != IDENTIFIER || EOL_flag != NO_EOL){
        return SYN_ERROR;
    }

    strCopyString(&(id_atr), &atr);
    //id :=
    token = get_token(GET, &EOL_flag, &atr);
    if (token == LEX_ERROR){
        return LEX_ERROR;
    }

    if ( token != DEFINITION || EOL_flag != NO_EOL){
        return SYN_ERROR;
        }

    retvalue = tablelist_insert(TL, &id_atr);
    
    //id := <expr>            
    if ((retvalue = expression(&expr_type,TL)) != TRUE){
        DEBUG_MODE("Chyba v expr")
        return retvalue;
    }


        if (retvalue == FALSE){
                DEBUG_MODE("Pokus o redefiniciu premennej")
                return SYN_ERROR;
        }

        tNode * tempnode = tablelist_search(TL, &id_atr);
        if (tempnode != NULL){
            tempnode->data.function = FALSE;
            tempnode->data.type = expr_type;
            int first = global_scope_num/100;
            char c = first + '0'; //global scope na char
            strAddChar(&(tempnode->code_gen_key), c);
            int second = global_scope_num/10;
            c = second + '0'; //global scope na char
            strAddChar(&(tempnode->code_gen_key), c);
            int third = global_scope_num%10;
            c = third + '0'; //global scope na char
            strAddChar(&(tempnode->code_gen_key), c);
        }
    strCopyString(&(codegen_name), &(tempnode->code_gen_key));
    codegen_declare(codegen_name.str);
        
    strFree(&codegen_name);
    return TRUE;
    DEBUG_MODE("FOR-DEFINITION")
}


//statement - return
int s_return(string funcname){
    int expr_type = EMPTY;
    int retvalue = EMPTY;
    int token;
    tReturn * tempreturn;
    DEBUG_MODE("STATEMENT: RETURN")
    tNode * funcnode = tablelist_search(TL, &funcname);
    token = get_token(PEEK, &EOL_flag, &atr);
    if (EOL_flag == EOL_FOUND){
        DEBUG_MODE("Return za sebou nema ziadne return parametre")
        if (funcnode->data.return_num != 0){
            DEBUG_MODE("Funkcia by mala nieco vratit")
            return PARAM_ERROR;
        }
        codegen_func_end();
        return TRUE;
    }
    //return <expr>
    if ((retvalue = expression(&expr_type,TL)) != TRUE){
        DEBUG_MODE("Chyba v expr")
        return retvalue;
    }



    tempreturn = get_return(GlobalTable, GlobalTable->root, &funcname, recursive_counter);
    if (tempreturn == NULL){
        DEBUG_MODE("Nadpocet return parametrov funkcie")
        return PARAM_ERROR;
    }

    if (tempreturn->type != expr_type){
        DEBUG_MODE("Nesuhlasi typ return parametru - pred implementovanim <expr>")
        return PARAM_ERROR;
    }

    token = get_token(PEEK, &EOL_flag, &atr);
    if (token == LEX_ERROR){
        return LEX_ERROR;
    }

    codegen_func_return(recursive_counter);

    //return <expr>, 
    if (token == COMA && EOL_flag == NO_EOL){
        recursive_counter++;
        token = get_token(GET, &EOL_flag, &atr); //realne nacitanie
        if ((retvalue = s_return(funcname)) != TRUE){
                DEBUG_MODE("Chyba v return")
                return retvalue;
            }
    }
    
    if(funcnode->data.return_num != recursive_counter){
        DEBUG_MODE("Za returnom nedostatocny pocet parametrov");
        return PARAM_ERROR;
    }
    return TRUE;
    
}

//if
int s_if(string funcname){
    int expr_type = EMPTY;
    int retvalue = EMPTY;
    if_label_num++;
    int label_num = if_label_num; //nastavi sa na 
    int token;
    DEBUG_MODE("STATEMENT IF")

    //if <expr>
    if ((retvalue = expression(&expr_type,TL)) != TRUE){
        DEBUG_MODE("Chyba v expr")
        return retvalue;
    }
    if (expr_type != BOOL){
        DEBUG_MODE("Za ifom nie je bool")
        return TYPE_ERROR;
    }

    codegen_if_start(label_num);

    token = get_token(GET, &EOL_flag, &atr);
    if (token == LEX_ERROR){
        return LEX_ERROR;
    }
    //if <expr> {
    if (token != L_BRACKET1 || EOL_flag != NO_EOL){
        DEBUG_MODE("Chyba { v if")
        return SYN_ERROR;
    }

    /*if <expr>{

    }
    */

    if ((retvalue = s_body(FALSE, funcname, 0)) != TRUE){
        DEBUG_MODE("Zla sekvencia prikazov v if")
        return retvalue;
    }

    codegen_else_skip(label_num);

    token = get_token(GET, &EOL_flag, &atr);
    if (token == LEX_ERROR){
        return LEX_ERROR;
    }

    if ((token != ELSE) || (EOL_flag != NO_EOL)){
        DEBUG_MODE("Chyba else v if")
        return SYN_ERROR;
    }

    codegen_else_label(label_num);

    token = get_token(GET, &EOL_flag, &atr);
    if (token == LEX_ERROR){
        return LEX_ERROR;
    }
    if (token != L_BRACKET1){
        DEBUG_MODE("Chyba { za else")
        return SYN_ERROR;
    }
       /*if <expr>{

    } else {

    }
    */

    if ((retvalue = s_body(FALSE, funcname, 0)) != TRUE){
            DEBUG_MODE("Zla sekvencia prikazov v else")
            return retvalue;
        }

    codegen_if_end(label_num);
    return TRUE;
}

/**
 * Rozhodovanie o tom, ci sa za id, id, id... = id nachadza vyraz alebo funkcia,
 * si vyziadal mierny odklon od LL gramatiky. Vyuzivana je aj semanticka analyza,
 * ktora nahliadne, ci je token ID definovany ako premenna (vyraz) alebo funkcia (volanie funkcie),
 * v pripade nedefinovane ID, kedy moze byt vyraz volanim dosial nedefinovanej premennej  
 */
int func_or_expr(int for_assign){
    int expr_type = EMPTY;
    int retvalue = EMPTY;
    int token = EMPTY;
    int mode = CHECK;
    tDef * tempdef;
    tNode * id_node;
    tNode *param_node;
    string id_name;
    string codegen_name;
    strInit(&id_name);
    strInit(&codegen_name);
    //jedno ID uz je nacitane
    if (saved_token == IDENTIFIER){
        if ((param_node = tablelist_search(TL, &saved_atr)) == NULL){
            DEBUG_MODE("Priradenie hodnoty neinicializovanej premennej")
            return SEM_ERROR;
        }
        strCopyString(&(codegen_name), &(param_node->code_gen_key));
        retvalue = deflist_add(deflist, param_node->data.type, &codegen_name);
    }
    else if (saved_token == UNDERLINE){
        deflist_add(deflist, NOT_KNOWN, &codegen_name);
    }
    int id_counter = 1;
    while(1){
        token = get_token(GET, &EOL_flag, &atr);
        if (token == LEX_ERROR){
            return LEX_ERROR;
        }                    
        //id, id
        if (token == IDENTIFIER || token == UNDERLINE){
            if (EOL_flag != NO_EOL){
                return SYN_ERROR;
            }

            if (token == IDENTIFIER){
                if ((param_node = tablelist_search(TL, &atr)) == NULL){
                    DEBUG_MODE("Priradenie hodnoty neinicializovanej premennej")
                    return SEM_ERROR;
                }

                strCopyString(&(codegen_name), &(param_node->code_gen_key));

                if(param_node->data.function == TRUE){
                    DEBUG_MODE("Vola sa id funkcie")
                    return SEM_ERROR; //?
                }

                retvalue = deflist_add(deflist, param_node->data.type, &codegen_name);
            }

            else if (token == UNDERLINE){
                deflist_add(deflist, NOT_KNOWN, &codegen_name);
            }

            id_counter++;

            token = get_token(GET, &EOL_flag, &atr);
            if (token == LEX_ERROR){
                return LEX_ERROR;
            }
                                
            //id, id = 
            if ((token == ASSIGN) && (EOL_flag == NO_EOL)){
                break;
            }

            //id, id, 
            else if (token != COMA || EOL_flag != NO_EOL){
                DEBUG_MODE("Za id, id... nieco zle")
                return SYN_ERROR;
            }
        }
                            
        else {
            DEBUG_MODE("Za id, sa nacitalo nieco blbe")
            return SYN_ERROR;
        }
    }
    
    //id, id... = ID 
    token = get_token(PEEK, &EOL_flag, &atr);
    if (token == IDENTIFIER && EOL_flag == NO_EOL){
        DEBUG_MODE("Skumam ID")
        strCopyString(&(id_name), &atr);

        id_node = tablelist_search(TL, &id_name); //pozriem, ako je definovane
        if (id_node == NULL){
            DEBUG_MODE("ID nie je definovane,musi byt funkcia, pokracujem s LOAD modom");
                mode = LOAD;
                retvalue = InsertNode(GlobalTable, GlobalTable->root, &id_name);
                id_node = tablelist_search(TL, &id_name);
                id_node->data.function = TRUE;
                id_node->defined = FALSE;
        }
                        
        if (id_node != NULL){
            if (id_node->data.function != TRUE){ //nie je to id funkcie, je to teda vyraz obsahujuci id 
                int count = 1; //pomocna premenna pre pocitanie pozicie v defliste
                            
                while (id_counter != 0){
                    if ((retvalue = expression(&expr_type,TL)) != TRUE){
                        DEBUG_MODE("Expr za viacnasobnym priradenim nespravna")
                        return retvalue;
                    }        
                    tempdef = deflist_get(deflist, count);

                    if (tempdef->token != NOT_KNOWN){ //priradenie do _
                        if (tempdef->token != expr_type){
                            DEBUG_MODE("Jeden z typov vo viacnasobnom priradeni nesedi/hrozi pri nedokoncenom <expr>/")
                            return OTHER_ERROR;
                        }
                    
                        codegen_assign(tempdef->name.str);
                    }
                    id_counter--;
                    count++;           
                    //bude treba dalsiu <expr>
                    if (id_counter != 0){
                        token = get_token(GET, &EOL_flag, &atr);
                        if (token == LEX_ERROR){
                            return LEX_ERROR;
                        }
                        if (token != COMA || EOL_flag != NO_EOL) {
                            DEBUG_MODE("Chyba , v rade expr, popr. zly pocet expr")
                            return OTHER_ERROR;
                        }  
                    }
                }
            DEBUG_MODE("Pocet id a <expr> sedi")   
            return TRUE; 
            }//premenna ako prve id 
        } 

        //bude to pravdepodobne volanie funkcie
        DEBUG_MODE("ID bude funkcia")
        if (for_assign == TRUE){
            DEBUG_MODE("Vo for nemoze byt priradena funkcia")
            return SEM_ERROR; //vo for nemoze byť priradenie funkcie
        }
        token = get_token(GET, &EOL_flag, &atr); //realne nacitam ID
        strCopyString(&(id_name), &atr); //ulozim do funcname atribut
        tReturn * tempreturn;
        tDef * tempdef;
        int count = 1;
        token = get_token(GET, &EOL_flag, &atr);
        if (token == LEX_ERROR){
                return LEX_ERROR;
        }

        if ( token != L_BRACKET || EOL_flag != NO_EOL) {
            DEBUG_MODE("Chyba ( za funkciou za jednoduchym zavolanim fukncie")
            return SEM_ERROR; //neinicializovana premenna vo vyraze
        } 

        if ((retvalue = f_params(id_name, mode)) != TRUE){
            DEBUG_MODE("Zle parametre funkcie s viacerymi return hodnotami")
            return retvalue;
        }
        codegen_call(id_name.str);

        if (mode == CHECK){
            if (id_node->data.return_num != deflist->count){ //viac return hodnot
                DEBUG_MODE("Semanticka chyba v return hodnotach funkcie s viacerymi return hodnotami")
                return PARAM_ERROR;
            }

            while (id_counter != 0){
                tempdef = deflist_get(deflist, count);
                if (tempdef->token != NOT_KNOWN){ //pokial je tam _, je to jedno
                    tempreturn = get_return(GlobalTable, GlobalTable->root, &id_name, count);
                    if(tempreturn->type != tempdef->token){
                        if (tempreturn->type == NOT_KNOWN){ //pokial som pri prvom nedefinovanom volani funkcie nacitala iba _
                            tempreturn->type = tempdef->token;
                        }
                        else {
                            DEBUG_MODE("Nesedi typ return hodnoty funkcie s viacerymi return hodnotami")
                            return PARAM_ERROR;
                        }
                    }
                    codegen_funcreturn(tempdef->name.str, count);
                }
                id_counter--;
                count++;  

            }
        }//mode = CHECK

        if (mode == LOAD){
            while (id_counter != 0){
                tempdef = deflist_get(deflist, count);
                tempreturn = add_return(GlobalTable, GlobalTable->root, &id_name);
                tempreturn->type = tempdef->token;
                id_counter--;
                count++; 
                codegen_funcreturn(tempdef->name.str, count); 
            }
        }

        DEBUG_MODE("Koniec cyklu s viacerymi return hodnotami")
            
    }

    else { //prvy token sa nerovna ID
        //id = <expr>
            
        //nie je to id funkcie 
        int count = 1; //pomocna premenna pre pocitanie pozicie v defliste
                            
        while (id_counter != 0){
            if ((retvalue = expression(&expr_type,TL)) != TRUE){
                DEBUG_MODE("Expr za viacnasobnym priradenim nespravna")
                return retvalue;
            }
                        
            tempdef = deflist_get(deflist, count);            
            if (tempdef->token != NOT_KNOWN){ //priradenie do _
                if (tempdef->token != expr_type){
                    DEBUG_MODE("Jeden z typov vo viacnasobnom priradeni nesedi")
                        return OTHER_ERROR;
                }

                codegen_assign(tempdef->name.str);
            }
                
            id_counter--;
            count++;
                                
            //bude treba dalsiu <expr>
            if (id_counter != 0){
                token = get_token(GET, &EOL_flag, &atr);
                if (token == LEX_ERROR){
                    return LEX_ERROR;
                }
                if (token != COMA || EOL_flag != NO_EOL) {
                    DEBUG_MODE("Chyba , v rade expr, popr. zly pocet expr")
                    return OTHER_ERROR;
                }  
            }
        }


        token = get_token(PEEK, &EOL_flag, &atr);
        if (token == COMA){ //<expr> este neskoncili
            return OTHER_ERROR;
        }
          
    }

    deflist_delete(deflist);
    return TRUE;
                    
} //func_or_expr                     
                            
                            


/**
 * Rozhodovanie o tom, ci sa za id = id nachadza vyraz alebo funkcia,
 * si vyziadal mierny odklon od LL gramatiky. Vyuzivana je aj semanticka analyza,
 * ktora nahliadne, ci je token ID definovany ako premenna (vyraz) alebo funkcia (volanie funkcie),
 * v pripade nedefinovane ID, kedy moze byt vyraz volanim dosial nedefinovanej premennej  
 */
int simple_func_or_expr(int for_assign){
    int expr_type = EMPTY;
    int retvalue = EMPTY;
   int token = get_token(PEEK, &EOL_flag, &atr); //PEEK
   int mode = CHECK;
   tNode * tempnode;
   tReturn * tempreturn;
   string id_name;
   string codegen_name;
   strInit(&codegen_name);
   strInit(&id_name);
   strCopyString(&(id_name), &atr);
   if (token == LEX_ERROR){
        return LEX_ERROR;
    }                       
    if (token == IDENTIFIER && EOL_flag == NO_EOL){
            DEBUG_MODE("Skumam ID")
        
            tNode * id_node = tablelist_search(TL, &atr); //pozriem, ako je definovane
            if (id_node == NULL){
                DEBUG_MODE("ID nie je definovane,musi byt funkcia, pokracujem s LOAD modom");
                mode = LOAD;
                retvalue = InsertNode(GlobalTable, GlobalTable->root, &atr);
                tempnode = tablelist_search(TL, &atr);
                tempnode->data.function = TRUE;
                tempnode->defined = FALSE;
            }

            if (id_node != NULL){
                if (id_node->data.function != TRUE){ //nie je to id funkcie 
                    DEBUG_MODE("Volana funkcia je premenna");
                    //id = <expr>
                    if ((retvalue = expression(&expr_type,TL)) != TRUE){
                        DEBUG_MODE("Chyba v jednoduchom priradeni v <expr>")
                        return retvalue;
                    }

                    if (saved_token == UNDERLINE){
                        return TRUE; //tam sa už nedá nič pokaziť
                    }


                    tempnode = tablelist_search(TL, &saved_atr);
                    strCopyString(&(codegen_name), &(tempnode->code_gen_key)); //uložím si meno na generáciu kódu
                    codegen_assign(codegen_name.str);

                    if (saved_token == IDENTIFIER){
                        if ((id_node = tablelist_search(TL, &saved_atr)) == NULL){
                            DEBUG_MODE("Priradenie hodnoty neinicializovanej premennej")
                            return SEM_ERROR;
                        }
                        if (id_node->data.type != expr_type){
                            DEBUG_MODE("Chyba v type priradzovanej hodnoty")
                            return OTHER_ERROR;
                        }

                    }

                    return TRUE;          
                }//premenna 
                
            } 


            //bude to pravdepodobne volanie funkcie
            if (for_assign == TRUE){
                DEBUG_MODE("Vo for nemoze byt priradena funkcia")
                return SEM_ERROR; //vo for nemoze byť priradenie funkcie
            }
            token = get_token(GET, &EOL_flag, &atr); //realne nacitam ID
            DEBUG_MODE("ID bude funkcia")

            if (mode == CHECK){
                tempreturn = get_return(GlobalTable, GlobalTable->root, &atr, 1);
                if (id_node->data.return_num != 1){ //viac return hodnot
                    DEBUG_MODE("Semanticka chyba v return hodnotach funkcie s 1 return hodnotou")
                    return PARAM_ERROR;
                }

                if (saved_token == IDENTIFIER){ //nie UNDERLINE
                    if ((tempnode = tablelist_search(TL, &saved_atr)) == NULL){ //vyhladanie ulozeneho id v tabulke hodnot
                        DEBUG_MODE("Priradenie hodnoty neinicializovanej premennej")
                        return SEM_ERROR;
                    }

                    if (tempreturn->type != tempnode->data.type){
                        if (tempreturn->type == NOT_KNOWN){ //pokial som pri prvom nedefinovanom volani funkcie nacitala iba _
                            tempreturn->type = tempnode->data.type;
                        }
                        else {
                            DEBUG_MODE("Nesedi typ return hodnoty funkcie s 1 return hodnotou")
                            return PARAM_ERROR;
                        }
                    }      
                }
            }//check

            if (mode == LOAD){
                tempreturn = add_return(GlobalTable, GlobalTable->root, &atr);

                if (saved_token == IDENTIFIER){ //nie UNDERLINE
                    if ((tempnode = tablelist_search(TL, &saved_atr)) == NULL){ //vyhladanie ulozeneho id v tabulke hodnot
                        DEBUG_MODE("Priradenie hodnoty neinicializovanej premennej")
                        return SEM_ERROR;
                    }

                    tempreturn->type = tempnode->data.type;
                }

                else if (saved_token == UNDERLINE){
                    tempreturn->type = NOT_KNOWN;
                }
            }//load



            token = get_token(GET, &EOL_flag, &atr);
            if (token == LEX_ERROR){
                return LEX_ERROR;
            }

            if ( token != L_BRACKET || EOL_flag != NO_EOL) {
                DEBUG_MODE("Chyba ( za funkciou za jednoduchym zavolanim fukncie")
                return SEM_ERROR;
            } 

            if ((retvalue = f_params(id_name, mode)) != TRUE){
                DEBUG_MODE("Zle parametre funkcie za jednoduchym zavolanim funkcie")
                return retvalue;
            }

            if (saved_token != UNDERLINE){

                tempnode = tablelist_search(TL, &saved_atr);
                strCopyString(&(codegen_name), &(tempnode->code_gen_key)); //uložím si meno na generáciu kódu
                codegen_call(id_name.str);
                codegen_funcreturn(codegen_name.str, 1);

            }

        }

        else { //prvy token sa nerovna ID
            //id = <expr>
            
            if ((retvalue = expression(&expr_type,TL)) != TRUE){
                DEBUG_MODE("Chyba v jednoduchom priradeni v <expr>")
                return retvalue;
            }
            if (saved_token == IDENTIFIER){
                if ((tempnode = tablelist_search(TL, &saved_atr)) == NULL){
                    DEBUG_MODE("Priradenie hodnoty neinicializovanej premennej")
                    return SEM_ERROR;
                }

                else if (tempnode->data.type != expr_type){
                    DEBUG_MODE("Chyba v type priradzovanej hodnoty")
                    return OTHER_ERROR;
                }

                tempnode = tablelist_search(TL, &saved_atr);
                strCopyString(&(codegen_name), &(tempnode->code_gen_key)); //uložím si meno na generáciu kódu
                codegen_assign(codegen_name.str);


            }


            DEBUG_MODE("Jednoduchy vyraz bez ID na zaciatku je spravny")
        }
        return TRUE;
}




int return_params(string func_name, int mode){
    int retvalue = EMPTY;
    tNode * funcnode = tablelist_search(TL, &func_name); 

    int token = get_token(GET, &EOL_flag, &atr);
    if (token == LEX_ERROR){
        return LEX_ERROR;
    } 
    if ((!is_type(token)) || EOL_flag != NO_EOL){
        return SYN_ERROR;
    }
    if (mode == LOAD){
        tReturn * tempreturn = add_return(GlobalTable, GlobalTable->root, &(funcnode->key));
        switch (token){
            case INT_T:
                tempreturn->type = INT;
                break;

            case FLOAT64:
                tempreturn->type = FLOAT_N;
                break;

            case STRING_T:
                tempreturn->type = STRING_LIT;
                break;
        }
    }

    if (mode == CHECK){
        int type_convert;
        if (recursive_counter > funcnode->data.return_num){
            DEBUG_MODE("Funkcia nema tolko argumentov")
            return PARAM_ERROR;
        }
        tReturn * tempreturn = get_return(GlobalTable, GlobalTable->root, &(funcnode->key), recursive_counter);
        if (tempreturn == NULL){
            DEBUG_MODE("Funkcia zavolana s nedostatocnym poctom argumentov")
            return PARAM_ERROR;
        }
        switch (token){ //priradenie spravnej hodnoty atr
            case INT_T:
                type_convert = INT;
                break;

            case FLOAT64:
                type_convert = FLOAT_N;
                break;

            case STRING_T:
                type_convert = STRING_LIT;
                break;
        }

        if (tempreturn->type == NOT_KNOWN){ //priradzovali sa do _
            tempreturn->type = type_convert;
        }

        else if (tempreturn->type != type_convert){
            DEBUG_MODE("Return hodnoty funkcie nesedia")
            return PARAM_ERROR;
        }
    }
    
    token = get_token(GET, &EOL_flag, &atr);
    if (token == LEX_ERROR){
        return LEX_ERROR;
    } 

    //func id (args) (type)
    if (token == R_BRACKET && EOL_flag == NO_EOL){
        //func ID (args) (return params)
        DEBUG_MODE("Koniec return parametrov najdeny")
        if (funcnode->data.return_num != recursive_counter){
            DEBUG_MODE("Nesedi pocet parametrov medzi volanim a definiciou funkcie")
            return PARAM_ERROR;
        }
        return TRUE;
    }

    //func ID (args) (type,
    else if (token != COMA || EOL_flag != NO_EOL){
        return SYN_ERROR;
    }
                 
    //func ID (args) (type, type
    DEBUG_MODE("Nacitany dalsi return parameter")
    recursive_counter++;
    if ((retvalue = return_params(func_name, mode)) != TRUE){
        DEBUG_MODE("Zly dalsi return parameter")
        return retvalue;
    }

    //prvy aj rekurzivne return_params ok 
    return TRUE;

}



// id(...
int f_params(string name, int mode){
    string codegen_name;
    strInit(&codegen_name);
    tNode * funcnode = tablelist_search(TL, &name);
    tNode * token_name;
    tParams * tempparam;

    int token= get_token(GET, &EOL_flag, &atr);
    if (token == LEX_ERROR){
        return LEX_ERROR;
    } 

    if (mode != PRINT_M){
        codegen_createframe();
    }
    //id ()
    if (token == R_BRACKET) {
        if (mode == LOAD){
            funcnode->data.param_num = 0;
        }

        else if (mode == CHECK) {
            if (funcnode->data.param_num != 0){
                DEBUG_MODE("Nesedia parametre volanej funkcie")
                return PARAM_ERROR;
            }
        }
        return TRUE;
    }

    
    if ((is_term(token) != TRUE || EOL_flag != NO_EOL)){
        DEBUG_MODE("Syntakticka chyba v parametroch funkcie")
        return SYN_ERROR;
    }

    if (token == UNDERLINE){
        DEBUG_MODE("V parametroch je _");
        return OTHER_ERROR;
    }

    int paramnum = 1;
    int tokennum = 1;
    if (mode == CHECK){
        if (tokennum <= funcnode->data.param_num){
            tempparam = get_param(GlobalTable, GlobalTable->root, &name, paramnum);
        }
        else {
            DEBUG_MODE("Nadbytocne parametre vo volani funkcie")
            return PARAM_ERROR;
        }
        
        if (token == IDENTIFIER){
            if ((token_name = tablelist_search(TL, &atr)) == NULL){ //vyhľadá sa aktuálny token, pokiaľ je to premenná
                
                DEBUG_MODE("Jeden z parametrov funkcie nie je inicializovany")
                return SEM_ERROR;
            }
            strCopyString(&(codegen_name), &(token_name->code_gen_key));
            if (token_name->data.function == TRUE){
                DEBUG_MODE("Jeden z parametrov je ID funkcie");
                return PARAM_ERROR;
            }

            if (token_name->data.type != tempparam->type){
                DEBUG_MODE("Nesedí typ prvého parametra volania funkcie")
                return PARAM_ERROR;
            }
        }
        else { //token je int, float, string...
            if(tempparam->type != token){
                DEBUG_MODE("Nesedi typ prvého parametra volania funkcie")
                return PARAM_ERROR;
            }
        }
    }

    else if (mode == LOAD){
        tempparam = add_param(GlobalTable, GlobalTable->root, &name);
        if (token == IDENTIFIER){
            if ((token_name = tablelist_search(TL, &atr)) == NULL){ //vyhľadá sa aktuálny token, pokiaľ je to premenná
                DEBUG_MODE("Jeden z parametrov funkcie nie je inicializovany")
                return SEM_ERROR;
            }
            strCopyString(&(codegen_name), &(token_name->code_gen_key));
            if (token_name->data.function == TRUE){
                DEBUG_MODE("Jeden z parametrov je ID funkcie");
                return PARAM_ERROR;
            }

            tempparam->type = token_name->data.type;
        }

        else { //token je int, float, string...
            tempparam->type = token;
        }
    }

    else if (mode == PRINT_M){
        if (token == IDENTIFIER){
            if ((token_name = tablelist_search(TL, &atr)) == NULL){ //vyhľadá sa aktuálny token, pokiaľ je to premenná
                DEBUG_MODE("Jeden z parametrov funkcie nie je inicializovany")
                return SEM_ERROR;
            }
            strCopyString(&(codegen_name), &(token_name->code_gen_key));
            if (token_name->data.function == TRUE){
                DEBUG_MODE("Jeden z parametrov je ID funkcie");
                return PARAM_ERROR;
            }

            codegen_write(token, codegen_name.str);
        }

        else {
            codegen_write(token, atr.str);
        }

    }

    if (mode != PRINT_M){
        if (token == IDENTIFIER){
            codegen_param(token, codegen_name.str, paramnum);
        }
        else {
            codegen_param(token, atr.str, paramnum);
        }
    }

    while(1){ 
        token = get_token(GET, &EOL_flag, &atr);
        if (token == LEX_ERROR){
            return LEX_ERROR;
        }
        //id (terms)
        if ((token == R_BRACKET) && (EOL_flag == NO_EOL)){
            if (mode == CHECK){
                if (funcnode->data.param_num != paramnum){
                    DEBUG_MODE("Nesedi pocet argumentov funkcie")
                    return PARAM_ERROR;
                }
            }
            return TRUE;
        }
        //id(term, 
         else if (token == COMA){
                token =  get_token(GET, &EOL_flag, &atr);
                if (token == LEX_ERROR){
                    return LEX_ERROR;
                }
                if ( (!is_term(token)) || (EOL_flag != NO_EOL) ){
                    return SYN_ERROR;
                }

                if (token == UNDERLINE){
                    DEBUG_MODE("V parametroch je _");
                    return OTHER_ERROR;
                }

                tokennum++;
                paramnum++;
                
                //dalsi term
                if (mode == CHECK){
                    if (tokennum <= funcnode->data.param_num){
                        tempparam = get_param(GlobalTable, GlobalTable->root, &name, paramnum);
                    }
                    else {
                        DEBUG_MODE("Nadbytocne parametre vo volani funkcie")
                        return PARAM_ERROR;
                    }
        
                    if (token == IDENTIFIER){
                        if ((token_name = tablelist_search(TL, &atr)) == NULL){ //vyhľadá sa aktuálny token, pokiaľ je to premenná
                            DEBUG_MODE("Jeden z parametrov funkcie nie je inicializovany")
                            return SEM_ERROR;
                        }
                        strCopyString(&(codegen_name), &(token_name->code_gen_key));
                        if (token_name->data.function == TRUE){
                            DEBUG_MODE("Jeden z parametrov je ID funkcie");
                            return PARAM_ERROR;
                        }

                        if (token_name->data.type != tempparam->type){
                            DEBUG_MODE("Nesedí typ ďalšieho parametra volania funkcie")
                            return PARAM_ERROR;
                        }
                    }
                    else { //token je int, float, string...
                        if(tempparam->type != token){
                            DEBUG_MODE("Nesedi typ prvého parametra volania funkcie")
                            return PARAM_ERROR;
                        }
                    }
                }

                else if (mode == LOAD){
                    tempparam = add_param(GlobalTable, GlobalTable->root, &name);
                    if (token == IDENTIFIER){
                        if ((token_name = tablelist_search(TL, &atr)) == NULL){ //vyhľadá sa aktuálny token, pokiaľ je to premenná
                            DEBUG_MODE("Jeden z parametrov funkcie nie je inicializovany")
                            return SEM_ERROR;
                        }
                        strCopyString(&(codegen_name), &(token_name->code_gen_key));
                        if (token_name->data.function == TRUE){
                            DEBUG_MODE("Jeden z parametrov je ID funkcie");
                            return PARAM_ERROR;
                        }

                        tempparam->type = token_name->data.type;
                    }

                    else { //token je int, float, string...
                        tempparam->type = token;
                    }
                }


                else if (mode == PRINT_M){
                    if (token == IDENTIFIER){
                        if ((token_name = tablelist_search(TL, &atr)) == NULL){ //vyhľadá sa aktuálny token, pokiaľ je to premenná
                            DEBUG_MODE("Jeden z parametrov funkcie nie je inicializovany")
                            return SEM_ERROR;
                        }
                        strCopyString(&(codegen_name), &(token_name->code_gen_key));
                        if (token_name->data.function == TRUE){
                            DEBUG_MODE("Jeden z parametrov je ID funkcie");
                            return PARAM_ERROR;
                        }

                        codegen_write(token, codegen_name.str);
                    }
                    else {
                        codegen_write(token, atr.str);
                    }
                }

                if (mode != PRINT_M){
                    if (token == IDENTIFIER){
                        codegen_param(token, codegen_name.str, paramnum);
                    }
                    else {
                        codegen_param(token, atr.str, paramnum);
                    }
                }



        } //coma

        //id(term ine
        else {
            DEBUG_MODE("Syntakticka chyba v parametroch")
            return SYN_ERROR;
        }
    } //while
}







//zacina func ID (type...
int args(string func_name, int mode){
    int retvalue = EMPTY;

    tNode * funcnode = tablelist_search(TL, &func_name); 
    tParams * tempparam;

    int token = get_token(GET, &EOL_flag, &atr);
    if (token == LEX_ERROR){
        return LEX_ERROR;
    }
    if (token != IDENTIFIER || EOL_flag != NO_EOL){
        DEBUG_MODE("Chyba v identifikatore argumentov funkcie")
        return SYN_ERROR;
    }

    if (mode == LOAD){
        tempparam = add_param(GlobalTable, GlobalTable->root, &(funcnode->key));
        strCopyString(&(tempparam->name), &atr); //skopirovanie nazvu argumentu
    }

    else if (mode == CHECK){
        if (recursive_counter > funcnode->data.param_num){
            DEBUG_MODE("Funkcia nema tolko argumentov")
            return PARAM_ERROR;
        }
        tempparam = get_param(GlobalTable, GlobalTable->root, &(funcnode->key), recursive_counter);
        if (tempparam == NULL){
            DEBUG_MODE("Funkcia nema dostatocny pocet argumentov")
            return PARAM_ERROR;
        }
        strCopyString(&(tempparam->name), &atr); //skopirovanie nazvu argumentu
        
    }

    //func id (id type
    token = get_token(GET, &EOL_flag, &atr);
    if (token == LEX_ERROR){
        return LEX_ERROR;
    }
    if((is_type(token) == FALSE) || EOL_flag != NO_EOL){
        DEBUG_MODE("Za id v argumentoch funkcie nie je typ");
        return SYN_ERROR;
    }

    if (mode == LOAD){
        switch (token){ //priradenie spravnej hodnoty atr
            case INT_T:
                tempparam->type = INT;
                break;

            case FLOAT64:
                tempparam->type = FLOAT_N;
                break;

            case STRING_T:
                tempparam->type = STRING_LIT;
                break;
        }
    }

    else if (mode == CHECK){
        int type_convert;
        switch (token){ //priradenie spravnej hodnoty atr
            case INT_T:
                type_convert = INT;
                break;

            case FLOAT64:
                type_convert = FLOAT_N;
                break;

            case STRING_T:
                type_convert = STRING_LIT;
                break;
        }

        if (tempparam->type != type_convert){
            DEBUG_MODE("Nesedi typ argumentu funkcie")
            return PARAM_ERROR;
        }

    }
    token = get_token(GET, &EOL_flag, &atr);

    //func id(id type, 
    if ((token == COMA) && (EOL_flag == NO_EOL)){
        recursive_counter++;
        if ((retvalue = args(func_name, mode)) != TRUE){
            DEBUG_MODE("Nespravny dalsi argument")
            return retvalue;
        } 

        DEBUG_MODE("Precitany dalsi argument funkcie")   
    }
    //func id (id type)
    else if (token == R_BRACKET && EOL_flag == NO_EOL){
        DEBUG_MODE("Najdeny koniec argumentov")
        if (funcnode->data.param_num != recursive_counter){
            DEBUG_MODE("Nesedi pocet parametrov medzi volanim a definiciou funkcie")
            return PARAM_ERROR;
        }
        return TRUE;
    }
    else {
        if (token == LEX_ERROR){
            return LEX_ERROR;
        }
        DEBUG_MODE("Neocakavany znak v argumentoch funkcie")
        return SYN_ERROR;
    }
    return TRUE;
}


int prolog(){
    int token = get_token(GET, &EOL_flag, &atr);      
    if (token != PACKAGE){
        return SYN_ERROR; 
    }
    //package main
    token = get_token(GET, &EOL_flag, &atr);
    if (token == LEX_ERROR){
        return LEX_ERROR;
    } 
    //package eolf-flag main
    if (token == IDENTIFIER && EOL_flag == NO_EOL){
        if ((strcmp(atr.str, "main") == 0)) {
            return TRUE;
        }
        DEBUG_MODE("Package nie je main"); 
    }
    return SYN_ERROR;
}

//PLACEHOLDER funkcia vyuzivana na testovanie zvysku gramatiky bez precedencnej analyzy
int expr(int * expr_type){
    int token = get_token(GET, &EOL_flag, &atr);
    if (token == LEX_ERROR){
        return LEX_ERROR;
    }

    *expr_type = token;

    if ((!is_term(token))){
        return SYN_ERROR;
    }

    return TRUE;
}


//vsunutie built-in funkcii do tabuľky symbolov
void built_in_insert(){
        string func_name;
    strInit(&func_name);
    tNode * temp;
    tParams * temparam;
    tReturn * tempreturn;

    //func inputs() (string,int)
    char name2[] = "inputs";
    copystr(name2, &func_name);
    tablelist_insert(TL, &func_name);
    temp = tablelist_search(TL, &func_name);
    if (temp == NULL){
        DEBUG_MODE("Internty problem");
        exit(-1);
    }
    temp->defined = TRUE;
    temp->data.function = TRUE;
    tempreturn = add_return(GlobalTable, GlobalTable->root, &func_name);
    tempreturn->type = STRING_LIT;
    tempreturn = add_return(GlobalTable, GlobalTable->root, &func_name);
    tempreturn->type = INT;
    strClear(&func_name);

    codegen_inputs();


    //func inputi() (int,int)
    char name3[] = "inputi";
    copystr(name3, &func_name);
    tablelist_insert(TL, &func_name);
    temp = tablelist_search(TL, &func_name);
    if (temp == NULL){
        DEBUG_MODE("Internty problem");
        exit(-1);
    }
    temp->defined = TRUE;
    temp->data.function = TRUE;
    tempreturn = add_return(GlobalTable, GlobalTable->root, &func_name);
    tempreturn->type = INT;
    tempreturn = add_return(GlobalTable, GlobalTable->root, &func_name);
    tempreturn->type = INT;
    strClear(&func_name);

    codegen_inputi();


    //func inputf() (float64,int)
    char name4[] = "inputf";
    copystr(name4, &func_name);
    tablelist_insert(TL, &func_name);
    temp = tablelist_search(TL, &func_name);
    if (temp == NULL){
        DEBUG_MODE("Internty problem");
        exit(-1);
    }
    temp->defined = TRUE;
    temp->data.function = TRUE;
    tempreturn = add_return(GlobalTable, GlobalTable->root, &func_name);
    tempreturn->type = FLOAT_N;
    tempreturn = add_return(GlobalTable, GlobalTable->root, &func_name);
    tempreturn->type = INT;
    strClear(&func_name); 
    codegen_inputf();

    //func int2float(i int) (float64)
    char name5[] = "int2float";
    copystr(name5, &func_name);
    tablelist_insert(TL, &func_name);
    temp = tablelist_search(TL, &func_name);
    if (temp == NULL){
        DEBUG_MODE("Internty problem");
        exit(-1);
    }
    temp->defined = TRUE;
    temp->data.function = TRUE;
    temparam = add_param(GlobalTable, GlobalTable->root, &func_name);
    temparam->type = INT;
    tempreturn = add_return(GlobalTable, GlobalTable->root, &func_name);
    tempreturn->type = FLOAT_N;
    strClear(&func_name); 
    codegen_int2float();

    //func float2int(f float64) (int)
    char name6[] = "float2int";
    copystr(name6, &func_name);
    tablelist_insert(TL, &func_name);
    temp = tablelist_search(TL, &func_name);
    if (temp == NULL){
        DEBUG_MODE("Internty problem");
        exit(-1);
    }
    temp->defined = TRUE;
    temp->data.function = TRUE;
    temparam = add_param(GlobalTable, GlobalTable->root, &func_name);
    temparam->type = FLOAT_N;
    tempreturn = add_return(GlobalTable, GlobalTable->root, &func_name);
    tempreturn->type = INT;
    strClear(&func_name); 

    codegen_float2int();

    //func len(𝑠 string) (int)
    char name7[] = "len";
    copystr(name7, &func_name);
    tablelist_insert(TL, &func_name);
    temp = tablelist_search(TL, &func_name);
    if (temp == NULL){
        DEBUG_MODE("Internty problem");
        exit(-1);
    }
    temp->defined = TRUE;
    temp->data.function = TRUE;
    temparam = add_param(GlobalTable, GlobalTable->root, &func_name);
    temparam->type = STRING_LIT;
    tempreturn = add_return(GlobalTable, GlobalTable->root, &func_name);
    tempreturn->type = INT;
    strClear(&func_name); 

    codegen_len();

    //func substr (s string, i int, n int) (string, int)
    char name8[] = "substr";
    copystr(name8, &func_name);
    tablelist_insert(TL, &func_name);
    temp = tablelist_search(TL, &func_name);
    if (temp == NULL){
        DEBUG_MODE("Internty problem");
        exit(-1);
    }
    temp->defined = TRUE;
    temp->data.function = TRUE;
    temparam = add_param(GlobalTable, GlobalTable->root, &func_name);
    temparam->type = STRING_LIT;
     temparam = add_param(GlobalTable, GlobalTable->root, &func_name);
    temparam->type = INT;
     temparam = add_param(GlobalTable, GlobalTable->root, &func_name);
    temparam->type = INT;
    tempreturn = add_return(GlobalTable, GlobalTable->root, &func_name);
    tempreturn->type = STRING_LIT;
    tempreturn = add_return(GlobalTable, GlobalTable->root, &func_name);
    tempreturn->type = INT;
    strClear(&func_name); 
    codegen_substr();

    //func ord(s string, i int) (int, int)
    char name9[] = "ord";
    copystr(name9, &func_name);
    tablelist_insert(TL, &func_name);
    temp = tablelist_search(TL, &func_name);
    if (temp == NULL){
        DEBUG_MODE("Internty problem");
        exit(-1);
    }
    temp->defined = TRUE;
    temp->data.function = TRUE;
    temparam = add_param(GlobalTable, GlobalTable->root, &func_name);
    temparam->type = STRING_LIT;
     temparam = add_param(GlobalTable, GlobalTable->root, &func_name);
    temparam->type = INT;
    tempreturn = add_return(GlobalTable, GlobalTable->root, &func_name);
    tempreturn->type = INT;
    tempreturn = add_return(GlobalTable, GlobalTable->root, &func_name);
    tempreturn->type = INT;
    strClear(&func_name); 
    codegen_ord();

    
    //func chr(i int) (string, int)
    char name10[] = "chr";
    copystr(name10, &func_name);
    tablelist_insert(TL, &func_name);
    temp = tablelist_search(TL, &func_name);
    if (temp == NULL){
        DEBUG_MODE("Internty problem");
        exit(-1);
    }
    temp->defined = TRUE;
    temp->data.function = TRUE;
    temparam = add_param(GlobalTable, GlobalTable->root, &func_name);
    temparam->type = INT;
    tempreturn = add_return(GlobalTable, GlobalTable->root, &func_name);
    tempreturn->type = STRING_LIT;
    tempreturn = add_return(GlobalTable, GlobalTable->root, &func_name);
    tempreturn->type = INT;
    strClear(&func_name); 
    codegen_chr();

    //func print
    char name11[] = "print";
    copystr(name11, &func_name);
    tablelist_insert(TL, &func_name);
    temp = tablelist_search(TL, &func_name);
    if (temp == NULL){
        DEBUG_MODE("Internty problem");
        exit(-1);
    }
    temp->defined = TRUE;
    temp->data.function = TRUE;
    strClear(&func_name); 
    
    strFree(&func_name);


    
    

}