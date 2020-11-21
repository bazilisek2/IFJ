#include <stdio.h>
#include <stdlib.h>
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
//#define SYNTAX_CHECK 
void print_token(int return_value);

int main() {

            int k;
            strInit(&atr);
            #ifdef SCANNER_CHECk
            do {
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
            #endif

            
            #ifdef SYNTAX_CHECK
            int retvalue = analysis_start();
            if (retvalue == 1){
                printf("Analyza presla\n");
            }

            else if (retvalue == 99){
                printf("Lex error v analyze\n");
            }
            else if (retvalue == 100){
                printf("Syntakticky error v analyze\n");
            }

            else {
                printf("Neznamy error v analyze\n");
            }
            #endif


        //test tabulky
        
        tSymbolTable * ST;
        tSymbolTable * GT;
        tNode * variable;
        tNode * variable2;
        tParams * test;
        ST = malloc(sizeof(tSymbolTable));
        GT = malloc(sizeof(tSymbolTable));
        strAddChar(&atr, 'a');

        InsertNode(ST, ST->root, &atr);
        int returnval = InsertNode(ST, ST->root, &atr);

        variable = tableSearch(ST, ST->root, &atr);
        

        //(variable->defined) = TRUE;

        returnval = is_defined(ST, variable, &atr);

        
        //test pridávania parametrov
        test = add_param(ST, ST->root, &atr);
        test = add_param(ST, ST->root, &atr);
        test = add_param(ST, ST->root, &atr);

        if (test != NULL){
            printf("%d\n", variable->data.param_num);
        }

        printf("%d\n", variable->data.p_first->next->next->order);

        test = get_param(ST, ST->root, &atr, 2);
        if (test != NULL){
            printf("%d\n", test->order);
        }
        

        







            
        }
        

void print_token(int return_value){
    switch (return_value){
        case 4:
            printf("Token type: FLOAT_N\n");
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
        default: 
            printf("Unexpected value of token\n");

        

    }
        

}
