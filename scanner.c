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
    ID_OR_UNERLINE,
    ZEROCHECK

} T_state;


static int buffer = EMPTY;
static string atr_buffer;
static int inicialized = 0;

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

    if (mode==END && inicialized != 0){
        strFree(&atr_buffer);
    }

    if (inicialized == 0){
        strInit(&atr_buffer);
        inicialized++;
    }


    if (buffer!= EMPTY){
        int return_value = buffer;
        if (mode == GET){
            buffer = EMPTY;
        }
        
        strCopyString(strtmp, &atr_buffer);
        return return_value;
    }

    T_state state = START;

    char hexc[3];
    int oprtmp=0;
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
                    strCopyString(&atr_buffer, strtmp);
                    buffer = END_OF_FILE;
                    }
                return (END_OF_FILE);
            }
            else if (c=='+') {
                if (mode == PEEK){
                    strCopyString(&atr_buffer, strtmp);
                    buffer = PLUS;
                    }
                return (PLUS);
            }
            else if (c=='-') {
                if (mode == PEEK){
                    buffer = MINUS;
                    strCopyString(&atr_buffer, strtmp);
                }
                return (MINUS);
            }
            else if (c=='*') {
                if (mode == PEEK){
                    buffer = MULTIPLICATION;
                    strCopyString(&atr_buffer, strtmp);
                    }
                return (MULTIPLICATION);
            }
            else if (c=='!' || c=='<' || c=='>') {
                state = OPERATOR;
                oprtmp = c;
                strAddChar(strtmp,c);
            }
            else if (c=='=') {
                state = EQ;
                strAddChar(strtmp,c);
            }
            else if (c=='(') {
                if (mode == PEEK){
                    buffer = L_BRACKET;
                    strCopyString(&atr_buffer, strtmp);
                    }
                return (L_BRACKET);
            }
            else if (c==')') {
                if (mode == PEEK){
                    buffer = R_BRACKET;
                    strCopyString(&atr_buffer, strtmp);
                    }
                return (R_BRACKET);
            }
            else if (c=='{') {
                if (mode == PEEK){
                    buffer = L_BRACKET1;
                    strCopyString(&atr_buffer, strtmp);
                    }
                return (L_BRACKET1);
            }
            else if (c=='}') {
                if (mode == PEEK){
                    buffer = R_BRACKET1;
                    strCopyString(&atr_buffer, strtmp);
                    }
                return (R_BRACKET1);
            }
            else if (c==',') {
                if (mode == PEEK){
                    buffer = COMA;
                    strCopyString(&atr_buffer, strtmp);
                    }
                return (COMA);
            }
            else if (c==';') {
                if (mode == PEEK){
                    buffer = UNDERSCORE;
                    strCopyString(&atr_buffer, strtmp);
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
                strAddChar(strtmp,c);
                if (c=='0') { 
                    state = ZEROCHECK;
                }
                else {
                    state = NUMBER;
                }
            }
            else {
                if (mode == PEEK){
                    buffer = LEX_ERROR;
                    strCopyString(&atr_buffer, strtmp);
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
                            strCopyString(&atr_buffer, strtmp);
                        }
                        return (ELSE);
                    }
                    else if (strCmpConstStr(strtmp,"float64")==0) {
                        if (mode == PEEK){
                            buffer = FLOAT64;
                            strCopyString(&atr_buffer, strtmp);
                        }
                        return (FLOAT64);
                    }
                    else if (strCmpConstStr(strtmp,"for")==0) {
                        if (mode == PEEK){
                            buffer = FOR;
                            strCopyString(&atr_buffer, strtmp);
                        }
                        return (FOR);
                    }
                    else if (strCmpConstStr(strtmp,"func")==0) {
                        if (mode == PEEK){
                            buffer = FUNC;
                            strCopyString(&atr_buffer, strtmp);
                        }
                        return (FUNC);
                    }
                    else if (strCmpConstStr(strtmp,"if")==0) {
                        if (mode == PEEK){
                            buffer = IF;
                            strCopyString(&atr_buffer, strtmp);
                        }
                        return (IF);
                    }
                    else if (strCmpConstStr(strtmp,"int")==0) {
                        if (mode == PEEK){
                            buffer = INT_T;
                            strCopyString(&atr_buffer, strtmp);
                        }
                        return (INT_T);
                    }
                    else if (strCmpConstStr(strtmp,"package")==0) {
                        if (mode == PEEK){
                            buffer = PACKAGE;
                            strCopyString(&atr_buffer, strtmp);
                        }
                        return (PACKAGE);
                    }
                    else if (strCmpConstStr(strtmp,"return")==0) {
                        if (mode == PEEK){
                            buffer = RETURN;
                            strCopyString(&atr_buffer, strtmp);
                        }
                        return (RETURN);
                    }
                    else if (strCmpConstStr(strtmp,"string")==0) {
                        if (mode == PEEK){
                            buffer = STRING_T;
                            strCopyString(&atr_buffer, strtmp);
                        }
                        return (STRING_T);
                    }
                    /*else if (strCmpConstStr(strtmp,"print")==0) {
                        if (mode == PEEK){
                            buffer = PRINT;
                        }
                        return (PRINT);
                    }
                    else if (strCmpConstStr(strtmp,"int2float")==0) {
                        if (mode == PEEK){
                            buffer = INT2FLOAT;
                        }
                        return (INT2FLOAT);
                    }
                    else if (strCmpConstStr(strtmp,"float2int")==0) {
                        if (mode == PEEK){
                            buffer = FLOAT2INT;
                        }
                        return (FLOAT2INT);
                    }
                    else if (strCmpConstStr(strtmp,"len")==0) {
                        if (mode == PEEK){
                            buffer = LEN;
                        }
                        return (LEN);
                    }
                    else if (strCmpConstStr(strtmp,"substr")==0) {
                        if (mode == PEEK){
                            buffer = SUBSTR;
                        }
                        return (SUBSTR);
                    }
                    else if (strCmpConstStr(strtmp,"ord")==0) {
                        if (mode == PEEK){
                            buffer = ORD;
                        }
                        return (ORD);
                    }
                    else if (strCmpConstStr(strtmp,"inputs")==0) {
                        if (mode == PEEK){
                            buffer = INPUTS;
                        }
                        return (INPUTS);
                    }
                    else if (strCmpConstStr(strtmp,"inputi")==0) {
                        if (mode == PEEK){
                            buffer = INPUTI;
                        }
                        return (INPUTI);
                    }
                    else if (strCmpConstStr(strtmp,"inputf")==0) {
                        if (mode == PEEK){
                            buffer = INPUTF;
                        }
                        return (INPUTF);
                    }*/
                    else {
                        if (mode == PEEK){
                            buffer = IDENTIFIER;
                            strCopyString(&atr_buffer, strtmp);
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
                        strCopyString(&atr_buffer, strtmp);
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
                        strCopyString(&atr_buffer, strtmp);
                    }
                    return (IDENTIFIER);
                }
            break;
            case ZEROCHECK:
                if (isdigit(c)) {
                    if (mode == PEEK){
                        buffer = LEX_ERROR;
                    }
                    return (LEX_ERROR);
                }
                else if (c=='.'){
                    strAddChar(strtmp,c);
                    state = FLOAT;
                }
                else {
                    ungetc(c, stdin);
                    if (mode == PEEK){
                        buffer = INT;
                        strCopyString(&atr_buffer, strtmp);
                    }
                    return INT;
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
                        strCopyString(&atr_buffer, strtmp);
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
                            strCopyString(&atr_buffer, strtmp);
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
                        strCopyString(&atr_buffer, strtmp);
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
                        strCopyString(&atr_buffer, strtmp);
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
                        buffer = FLOAT_N;
                        strCopyString(&atr_buffer, strtmp);
                    }
                    return (FLOAT_N);
                }
            break;


            case STRING:
                if (c=='"') {
                    if (mode == PEEK){
                        buffer = STRING_LIT;
                        strCopyString(&atr_buffer, strtmp);
                    }
                    return (STRING_LIT);
                }
                else if (c=='\\') {
                    state = ESCAPE_SEQ;
                }    
                else if (c==' ') {
                    strAddChar(strtmp,'\\');
                    strAddChar(strtmp,'0');
                    strAddChar(strtmp,'3');
                    strAddChar(strtmp,'2');
                    state = STRING;
                }                   
                else if (c>31) {
                    strAddChar(strtmp,c);
                }
                else  {
                    if (mode == PEEK){
                        buffer = LEX_ERROR;
                        strCopyString(&atr_buffer, strtmp);
                    }
                    return (LEX_ERROR);
                }
                break;

            case ESCAPE_SEQ:
                if (c=='t'){
                    strAddChar(strtmp,'\\');
                    strAddChar(strtmp,'0');
                    strAddChar(strtmp,'0');
                    strAddChar(strtmp,'9');
                    state = STRING;
                }
                else if (c=='n'){
                    strAddChar(strtmp,'\\');
                    strAddChar(strtmp,'0');
                    strAddChar(strtmp,'1');
                    strAddChar(strtmp,'0');
                    state = STRING;
                }
                else if (c=='\\'){
                    strAddChar(strtmp,'\\');
                    strAddChar(strtmp,'0');
                    strAddChar(strtmp,'9');
                    strAddChar(strtmp,'2');
                    state = STRING;
                }
                else if (c=='"') {
                    strAddChar(strtmp,'\\');
                    strAddChar(strtmp,'0');
                    strAddChar(strtmp,'3');
                    strAddChar(strtmp,'4');
                    state = STRING;
                }
                else if (c=='x') {
                    state = STRING_HEX;
                }
                else {
                    if (mode == PEEK){
                        buffer = LEX_ERROR;
                        strCopyString(&atr_buffer, strtmp);
                    }
                    return (LEX_ERROR);
                }
                break;

            case STRING_HEX:
                if (i<2 && isxdigit(c)) {
                    hexc[i]=c;
                    i++;

                }
                else if (i<2) {
                    if (mode == PEEK){
                        buffer = LEX_ERROR;
                    }
                    return (LEX_ERROR);
                }
                else {
                    hexc[i] = '\0';
                    hexo = strtol(hexc,NULL,16);
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
                        strCopyString(&atr_buffer, strtmp);
                    }
                    return (EQUAL);
                }
                else {
                    ungetc(c,stdin);
                    if (mode == PEEK){
                        buffer = ASSIGN;
                        strCopyString(&atr_buffer, strtmp);
                    }
                    return (ASSIGN);
                }

            case COLON:
                if (c=='=') {
                    if (mode == PEEK){
                        buffer = DEFINITION;
                        strCopyString(&atr_buffer, strtmp);
                    }
                    return DEFINITION;
                }
                else {
                    ungetc(c,stdin);
                    if (mode == PEEK){
                        buffer = LEX_ERROR;
                        strCopyString(&atr_buffer, strtmp);
                    }
                    return LEX_ERROR;
                }



            case OPERATOR:
                strAddChar(strtmp,c);
                if (strCmpConstStr(strtmp,"<=")==0) {
                    if (mode == PEEK){
                        buffer = LESSEQUAL;
                        strCopyString(&atr_buffer, strtmp);
                    }
                    return (LESSEQUAL);
                }
                else if (strCmpConstStr(strtmp,">=")==0) {
                    if (mode == PEEK){
                        buffer = GREATEQUAL;
                        strCopyString(&atr_buffer, strtmp);
                    }
                    return (GREATEQUAL);
                }
                else if (strCmpConstStr(strtmp,"!=")==0) {
                    if (mode == PEEK){
                        buffer = NOTEQUAL;
                        strCopyString(&atr_buffer, strtmp);
                    }
                    return (NOTEQUAL);
                }
                else if (oprtmp =='<') {
                    ungetc(c,stdin);
                    if (mode == PEEK){
                        buffer = LESS;
                        strCopyString(&atr_buffer, strtmp);
                    }
                    return (LESS);
                } 
                else if (oprtmp =='>') {
                    ungetc(c,stdin);
                    if (mode == PEEK){
                        buffer = GREAT;
                        strCopyString(&atr_buffer, strtmp);
                    }
                    return (GREAT); 
                }    
                else {
                    if (mode == PEEK){
                        buffer = LEX_ERROR;
                        strCopyString(&atr_buffer, strtmp);
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
                        strCopyString(&atr_buffer, strtmp);
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
            if (c == '\n'){
                *flag = EOL_FOUND;
            }

            if (c==EOF) {
               if (mode == PEEK){
                    buffer = LEX_ERROR;
                    strCopyString(&atr_buffer, strtmp);
                }
                return (LEX_ERROR);
            }
            break;

            case BLOCK_COMMENT_END:
            if (c=='/') {
                state = START;
            }
            else {
                
                state = BLOCK_COMMENT;

            }
            break;

            default:
                break;


        }
        }
        return 1;

        }