/****
 * IFJ Projekt 2020
 * 
 * Autori:
 * xjorio00 - Vanessa Jóriová 
 * 
 * Súhrn: spúšťanie syntaktickej analýzy a vývojové testovanie
 * 
 * */




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scanner.h"
#include "syntax_analysis.h"
#include "str.h"
#include "symtable.h"
#include "global.h"

#define GET 0
#define PEEK 1

int e_flag = 0;
string atr;

//#define SCANNER_CHECk 
#define SYNTAX_CHECK 
//#define STRUCT_CHECK
void print_token(int return_value);



int main() {
    

    


#ifdef SCANNER_CHECk
    do {
                int k;
                strInit(&atr);
                k = get_token(GET, &e_flag, &atr);
                printf("Precitany token: %s\n", atr.str);
                printf("%d:  ", k);
                print_token(k);
                printf("Flag je: %d\n", e_flag);
                if (find_eol()== 1) {
                printf("Najdeny koniec riadku\n");
            }
                if (k == LEX_ERROR) {
                    break;
                }

            }
            while(k != END_OF_FILE);
    
    k = get_token(END, &e_flag, &atr);
    strFree(&atr);
            
#endif

    

#ifdef SYNTAX_CHECK
    int retvalue = analysis_start();
            if (retvalue == 1){
                DEBUG_MODE("Analyza presla\n");
                return 0;
            }

            else if (retvalue == 99){
                DEBUG_MODE("Lex error v analyze\n");
                return 1;
            }
            else if (retvalue == 100){
                DEBUG_MODE("Syntakticky error v analyze\n");
                return 2;
            }

            else if(retvalue == 101){
                DEBUG_MODE("Semanticka chyba 3 v analyze\n");
                return 3;
            }
            
            else if(retvalue == 102){
                DEBUG_MODE("Semanticka chyba 6 v analyze\n");
                return 6;
            }
            else if(retvalue == 103){
                DEBUG_MODE("Semanticka chyba 5 v analyze\n");
                return 5;

            }
            else if(retvalue == 104){
                DEBUG_MODE("Semanticka chyba 7 v analyze\n");
                return 7;
            }
            else {
                printf("Neznamy error v analyze\n");
            }

#endif

#ifdef STRUCT_CHECK
    //test tabulky

    tSymbolTable * ST;
    tSymbolTable * GT;
    

    ST = malloc(sizeof(tSymbolTable));
    GT = malloc(sizeof(tSymbolTable));
    
    tableInit(ST);
    tableInit(GT);
    
    tNode * variable;
    tNode * variable2;
    tParams * test;
    strAddChar(&atr, 'a');

    string atr2;
    strInit(&atr2);
    strAddChar(&atr2, 'b');


    tTableList * TableList = malloc(sizeof(tTableList));
    tablelist_init(TableList);

    tablelist_add(TableList, GT);
    tablelist_insert(TableList, &atr);
    if (tablelist_search(TableList, &atr) != NULL){
        printf("Úspech\n");
    }

    int returnval = tablelist_insert(TableList, &atr);
    if (returnval == FALSE){
        printf("Úspech 2\n");
    }

    tablelist_add(TableList, ST);
    returnval = tablelist_insert(TableList, &atr);
    if (returnval == TRUE){
        printf("Úspech 3\n");
    }

    returnval = tablelist_insert(TableList, &atr);
    if (returnval == FALSE){
        printf("Úspech 4\n");
    }

    variable = tablelist_search(TableList, &atr);
    if (variable != NULL){
        printf("Úspech 5\n");
    }

    variable->data.param_num = 4;

    //mali by byt 4
    variable2 = tablelist_search(TableList, &atr);
    if (variable2 != NULL){
        printf("%d\n", variable->data.param_num);
    }

    //odstráni sa posledná tabulka
    tablelist_pop(TableList);

    
    variable = tablelist_search(TableList, &atr);
    if (variable != NULL){
        printf("Úspech 6\n");
    }

    //vráti paramnum prvej variable (EMPTY -> - 12);
    printf("%d\n", variable->data.param_num);

   tablelist_free(TableList);
   strFree(&atr);
   strFree(&atr2);
   free(TableList);
   free(ST);




    /*printf("Deflist test\n");
    tDefList * deflist = malloc(sizeof(tDefList));
    deflist_init(deflist);
    deflist_add(deflist, 1);
    deflist_add(deflist, 2);
    deflist_add(deflist, 3);
    tDef * temp = deflist_get(deflist, 2);
    printf("%d\n", temp->token); 
    temp = deflist_get(deflist, 1);
    printf("%d\n", temp->token); 
    printf("%d\n", deflist->count);
    deflist_add(deflist, 3);
    printf("%d\n", deflist->count);*/

    



#endif


}


void print_token(int return_value){
    switch (return_value){
        case 4:
            printf("Token type: FLOAT_N\n");
            break;
        case 8:
            printf("Token type: GREAT\n");
            break;    
        case 9:
            printf("Token type: LESS\n");
            break;    
        case 10:
            printf("Token type: PLUS\n");
            break;
        case 11:
            printf("Token type: MINUS\n");
            break;
        case 12:
            printf("Token type: MULTIPLICATION\n");
            break;
        case 13:
            printf("Token type: EQUAL\n");
            break;
        case 14:
            printf("Token type: ASSIGN\n");
            break;
        case 15:
            printf("Token type: DIVISION\n");
            break;
        case 16:
            printf("Token type: LESSEQUAL\n");
            break;
        case 17:
            printf("Token type: GREATEQUAL\n");
            break;
        case 18:
            printf("Token type: NOTEQUAL\n");
            break;
        case 19:
            printf("Token type: DEFINITION\n");
            break;
        case 21:
            printf("Token type: L_BRACKET\n");
            break;
        case 22:
            printf("Token type: R_BRACKET\n");
            break;
        case 23:
            printf("Token type: L_BRACKET1\n");
            break;
        case 24:
            printf("Token type: R_BRACKET1\n");
            break;
        case 25:
            printf("Token type: COMA\n");
            break;
        case 26:
            printf("Token type: UNDERSCORE\n");
            break;
        case 27:
            printf("Token type: UNDERLINE\n");
            break;
        case 30:
            printf("Token type: ELSE\n");
            break;
        case 31:
            printf("Token type: FLOAT64\n");
            break;
        case 32:
            printf("Token type: FOR\n");
            break;
        case 33:
            printf("Token type: FUNC\n");
            break;
        case 34:
            printf("Token type: IF\n");
            break;
        case 35:
            printf("Token type: PACKAGE\n");
            break;
        case 36:
            printf("Token type: RETURN\n");
            break;
        case 40:
            printf("Token type: INT\n");
            break;
        case 41:
            printf("Token type: FLOAT_L\n");
            break;
        case 42:
            printf("Token type: STRING_LIT\n");
            break;
        case 43:
            printf("Token type: STRING_T\n");
            break;
        case 44:
            printf("Token type: INT_T\n");
            break;
        case 50:
            printf("Token type: EOL\n");
            break;
        case 99:
            printf("LEX ERROR\n");
            break;
        case 60:
            printf("Token type: IDENTIFIER\n");
            break;
        case 61:
            printf("Token type: END_OF_FILE\n");
            break;
        case 70:
            printf("Token type: PRINT\n");
            break;
        case 71:
            printf("Token type: INT2FLOAT\n");
            break;
        case 72:
            printf("Token type: LEN\n");
            break;
        case 73:
            printf("Token type: SUBSTR\n");
            break;
        case 74:
            printf("Token type: ORD\n");
            break;
        case 75:
            printf("Token type: INPUTS\n");
            break;
        case 76:
            printf("Token type: INPUTI\n");
            break;
        case 77:
            printf("Token type: INPUTF\n");
            break;
        case 78:
            printf("Token type: FLOAT2INT\n");
            break;
        default:
            printf("Unexpected value of token\n");



    }


}
