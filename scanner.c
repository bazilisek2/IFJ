#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "scanner.h"
#include "global.h"
#include "str.h"

typedef enum{
    START,
    ID_KW,
    ID,
    NUMBER,
    FLOAT,
    EXPONENT_SIGN,
    EXPONENT_CHECK,
    EXPONENT,
    STRING,
    ESCAPE_SEQ,
    STRING_HEX,
    EQ,
    COLON,
    OPERATOR,
    COMMENT_OPERATOR,
    LINE_COMMENT,
    BLOCK_COMMENT,
    BLOCK_COMMENT_END,
    ID_OR_UNERLINE

} T_state;


static int buffer;

//zistí, či sa ZA znakom nachádza EOL (po jeho nájdení ho tam vráti kvôli flagu, ktorý vyhodnocuje EOL pred)
//TODO: ako EOL sa ráta aj viacriadkový komentár
int find_eol(){
    int c = fgetc(stdin);
    while (isspace(c)){
        if (c == '\n') {
            ungetc(c, stdin);
            return TRUE;
        }
        c = fgetc(stdin);
    }

    if (!isspace(c)){
        ungetc(c, stdin);
    }
    return FALSE;
}



//v mode GET token buffer rovno vrati, PEEK ho ulozi a vrati pri dalsom zavolani 
int get_token(int mode, int * flag, string *strtmp)

{   
    if (buffer!= EMPTY){
        int return_value = buffer;
        if (mode == GET){
            buffer = EMPTY;
        }
        return return_value;
    }

    T_state state = START;
    
    char hexc[3];
    int hexo=0;
    int c;
    int i=0;
    int floatcheck=0;
    *flag = NO_EOL;

    strClear(strtmp);   
    
    
    while (1)
    {
        
        c = fgetc(stdin);
        
        switch (state)
        {
            case START:
            if (isspace(c)) {
                if (c == '\n'){
                    *flag = EOL_FOUND; 
                }
                state = START;
            }
            else if (c == '\n') {
                if (mode == PEEK){
                    buffer = EOL;
                    } 
                return EOL;
            }
            else if (c=='/') {
                state = COMMENT_OPERATOR;
            }
            else if (c==EOF) {
                if (mode == PEEK){
                    buffer = END_OF_FILE;
                    } 
                return (END_OF_FILE);
            }
            else if (c=='+') {
                if (mode == PEEK){
                    buffer = PLUS;
                    } 
                return (PLUS);
            }
            else if (c=='-') {
                if (mode == PEEK){
                    buffer = MINUS;
                    } 
                return (MINUS);
            }
            else if (c=='*') {
                if (mode == PEEK){
                    buffer = MULTIPLICATION;
                    } 
                return (MULTIPLICATION);
            }
            else if (c=='!' || c=='<' || c=='>') {
                state = OPERATOR;
                strAddChar(strtmp,c);
            }
            else if (c=='=') {
                state = EQ;
                strAddChar(strtmp,c);
            }
            else if (c=='(') {
                if (mode == PEEK){
                    buffer = L_BRACKET;
                    } 
                return (L_BRACKET);
            }
            else if (c==')') {
                if (mode == PEEK){
                    buffer = R_BRACKET;
                    } 
                return (R_BRACKET);
            }
            else if (c=='{') {
                if (mode == PEEK){
                    buffer = L_BRACKET1;
                    } 
                return (L_BRACKET1);
            }
            else if (c=='}') {
                if (mode == PEEK){
                    buffer = R_BRACKET1;
                    } 
                return (R_BRACKET1);
            }
            else if (c==',') {
                if (mode == PEEK){
                    buffer = COMA;
                    } 
                return (COMA);
            }
            else if (c==';') {
                if (mode == PEEK){
                    buffer = UNDERSCORE;
                    } 
                return (UNDERSCORE);
            }
            else if (c=='_') {
                state = ID_OR_UNERLINE;
                strAddChar(strtmp,c);
            }
            else if (isalpha(c)) {
                state = ID_KW;
                strAddChar(strtmp,c);
            }

            else if (c=='"') {
                state = STRING;
            }
            else if (c==':') {
                state = COLON;
            }
            else if (isdigit(c)) {
                state = NUMBER;
                strAddChar(strtmp,c);
            }
            else {
                if (mode == PEEK){
                    buffer = LEX_ERROR;
                    } 
                return LEX_ERROR;
            }
            break;

            case ID_KW:
                if (c=='_') {
                    state = ID;
                }
                else if (isalnum(c)) {
                    strAddChar(strtmp,c);
                }
                else {
                    ungetc(c,stdin);
                    if (strCmpConstStr(strtmp,"else")==0) {
                        if (mode == PEEK){
                            buffer = ELSE;
                        } 
                        return (ELSE);
                    }
                    else if (strCmpConstStr(strtmp,"float64")==0) {
                        if (mode == PEEK){
                            buffer = FLOAT64;
                        }                         
                        return (FLOAT64);
                    }
                    else if (strCmpConstStr(strtmp,"for")==0) {
                        if (mode == PEEK){
                            buffer = FOR;
                        }     
                        return (FOR);
                    }
                    else if (strCmpConstStr(strtmp,"func")==0) {
                        if (mode == PEEK){
                            buffer = FUNC;
                        }                             
                        return (FUNC);
                    }
                    else if (strCmpConstStr(strtmp,"if")==0) {
                        if (mode == PEEK){
                            buffer = IF;
                        }  
                        return (IF);
                    }
                    else if (strCmpConstStr(strtmp,"int")==0) {
                        if (mode == PEEK){
                            buffer = INT_T;
                        }
                        return (INT_T);
                    }
                    else if (strCmpConstStr(strtmp,"package")==0) {
                        if (mode == PEEK){
                            buffer = PACKAGE;
                        }
                        return (PACKAGE);
                    }
                    else if (strCmpConstStr(strtmp,"return")==0) {
                        if (mode == PEEK){
                            buffer = RETURN;
                        }                        
                        return (RETURN);
                    }
                    else if (strCmpConstStr(strtmp,"string")==0) {
                        if (mode == PEEK){
                            buffer = STRING_T;
                        }                        
                        return (STRING_T);
                    }
                    else {
                        if (mode == PEEK){
                            buffer = IDENTIFIER;
                        }   
                        return (IDENTIFIER);
                    }
                }
                break;

            case ID_OR_UNERLINE:
                if (c == '_' || isalnum(c)) {
                    strAddChar(strtmp,c);
                    state = ID;
                }
                else {
                    ungetc(c,stdin);
                    if (mode == PEEK){
                        buffer = UNDERLINE;
                    }
                    return (UNDERLINE);
                }
            break;



            case ID:
                if (c == '_' || isalnum(c)) {
                    strAddChar(strtmp,c);
                }
                else {
                    ungetc(c,stdin);
                    if (mode == PEEK){
                        buffer = IDENTIFIER;
                    }
                    return (IDENTIFIER);
                }
            break;
            case NUMBER:
                if (isdigit(c)) {
                    strAddChar(strtmp,c);
                    state = NUMBER;
                }
                else if (c=='.') {
                    strAddChar(strtmp,c);
                    state = FLOAT;
                }
                else if (c == 'E' || c == 'e') {
                    strAddChar(strtmp, c);
                    state = EXPONENT_SIGN;
                }
                else {
                    ungetc(c, stdin);
                    if (mode == PEEK){
                        buffer = INT;
                    }   
                    return INT;
                }
            break;

            case FLOAT:
                if (isdigit(c)) {
                    strAddChar(strtmp,c);
                    floatcheck++;
                }
                else if (c == 'E' || c == 'e') {
                    strAddChar(strtmp, c);
                    state = EXPONENT_SIGN;
                }
                else {
                    if (floatcheck>0) {
                        ungetc(c,stdin);
                        if (mode == PEEK){
                            buffer = FLOAT_N;
                        }
                        return (FLOAT_N);
                    }
                    else {
                        if (mode == PEEK){
                        buffer = LEX_ERROR;
                    }
                    return (LEX_ERROR);    
                    }
                }
            break;

            case EXPONENT_SIGN:
                if (isdigit(c)) {
                    strAddChar(strtmp,c);
                    state = EXPONENT;
                }
                else if (c=='+' || c=='-') {
                    strAddChar(strtmp,c);
                    state = EXPONENT_CHECK;
                }
                else {
                    if (mode == PEEK){
                        buffer = LEX_ERROR;
                    }
                    return (LEX_ERROR);
                }
             break;

            case EXPONENT_CHECK:
                if (isdigit(c)) {
                    strAddChar(strtmp,c);
                    state = EXPONENT;
                }
                else {
                    if (mode == PEEK){
                        buffer = LEX_ERROR;
                    }
                    return (LEX_ERROR);
                }
            break;

            case EXPONENT:
                if (isdigit(c)) {
                    strAddChar(strtmp, c);
                }
                else {
                    ungetc (c,stdin);
                    if (mode == PEEK){
                        buffer = FLOAT_L;
                    }
                    return (FLOAT_L);
                }
            break;


            case STRING:
                if (c=='"') {
                    if (mode == PEEK){
                        buffer = STRING_LIT;
                    }
                    return (STRING_LIT);
                }
                else if (c=='\\') {
                    state = ESCAPE_SEQ;
                }
                else if (c>31) {
                    strAddChar(strtmp,c);
                }
                else  {
                    if (mode == PEEK){
                        buffer = LEX_ERROR;
                    }
                    return (LEX_ERROR);
                }
                break;

            case ESCAPE_SEQ:
                if (c=='t'){
                    strAddChar(strtmp,'\t');
                    state = STRING;
                }
                else if (c=='n'){
                    strAddChar(strtmp,'\n');
                    state = STRING;
                }
                else if (c=='\\'){
                    strAddChar(strtmp,'\\');
                    state = STRING;
                }
                else if (c=='"') {
                    strAddChar(strtmp, '\"');
                    state = STRING;
                }
                else if (c=='x') {
                    state = STRING_HEX;
                }
                else {
                    if (mode == PEEK){
                        buffer = LEX_ERROR;
                    }
                    return (LEX_ERROR);
                }
                break;

            case STRING_HEX:
                if (i<2 && isdigit(c)) {
                    hexc[i]=c;
                    i++;

                }
                else if (i==0) {
                    if (mode == PEEK){
                        buffer = LEX_ERROR;
                    }
                    return (LEX_ERROR);
                }
                else {
                    hexc[i] = '\0';
                    hexo = strtol(hexc,NULL,16);
                    if (hexo == 0) {
                        if (mode == PEEK){
                            buffer = LEX_ERROR;
                    }
                        return (LEX_ERROR);
                    }
                    strAddChar(strtmp,hexo);
                    i=0;
                    ungetc(c, stdin);
                    state = STRING;
                }
                break;

            case EQ:
                if (c=='=') {
                    if (mode == PEEK){
                        buffer = EQUAL;
                    }
                    return (EQUAL);
                }
                else {
                    ungetc(c,stdin);
                    if (mode == PEEK){
                        buffer = ASSIGN;
                    }
                    return (ASSIGN);
                }

            case COLON:
                if (c=='=') {
                    if (mode == PEEK){
                        buffer = DEFINITION;
                    }
                    return DEFINITION;
                }
                else {
                    ungetc(c,stdin);
                    if (mode == PEEK){
                        buffer = LEX_ERROR;
                    }
                    return LEX_ERROR;
                }



            case OPERATOR:
                strAddChar(strtmp,c);
                if (strCmpConstStr(strtmp,"<=")==0) {
                    if (mode == PEEK){
                        buffer = LESSEQUAL;
                    }
                    return (LESSEQUAL);
                }
                else if (strCmpConstStr(strtmp,">=")==0) {
                    if (mode == PEEK){
                        buffer = GREATEQUAL;
                    }
                    return (GREATEQUAL);
                }
                else if (strCmpConstStr(strtmp,"!=")==0) {
                    if (mode == PEEK){
                        buffer = NOTEQUAL;
                    }
                    return (NOTEQUAL);
                }
                else {
                    if (mode == PEEK){
                        buffer = LEX_ERROR;
                    }
                    return (LEX_ERROR);
                }

            case COMMENT_OPERATOR:
            if (c=='/') {
                state = LINE_COMMENT;
            }
            else if (c=='*') {
                state = BLOCK_COMMENT;
            }
            else {
                ungetc (c,stdin);
                if (mode == PEEK){
                        buffer = DIVISION;
                    }
                return (DIVISION);
            }
            break;

            case LINE_COMMENT:
            if (c == '\n') {
                state = START;
            }
            break;

            case BLOCK_COMMENT:
            if (c=='*') {
                state = BLOCK_COMMENT_END;
            }
            break;

            case BLOCK_COMMENT_END:
            if (c=='/') {
                state = START;
            }
            else {
                if (c == '\n'){
                    *flag = EOL_FOUND;
                }
                state = BLOCK_COMMENT;

            }
            break;

            default:
                break;


        }
        }
        return 1;

        }
