#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "str.h"
#define PLUS 10
#define MINUS 11
#define MULTIPLICATION 12
#define EQUAL 13
#define ASSIGN 14
#define DIVISION 15
#define LESSEQUAL 16
#define GREATEQUAL 17
#define NOTEQUAL 18
#define L_BRACKET 21
#define R_BRACKET 22
#define L_BRACKET1 23
#define R_BRACKET1 24
#define COMA 25
#define UNDERSCORE 26
#define ELSE 30
#define FLOAT64 31
#define FOR 32
#define FUNC 33
#define IF 34
#define PACKAGE 35
#define RETURN 36
#define INT 40
#define STRING_LIT 41
#define EOL 50
#define LEX_ERROR 99
string *strtmp;
int get_token()

{
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
        OPERATOR,
        COMMENT_OPERATOR,
        LINE_COMMENT,
        BLOCK_COMMENT,
        BLOCK_COMMENT_END

    } T_state;
    T_state state = START;
    char hexc[3];
    int hexo=0;
    int c;
    int i=0;
    strClear(strtmp);
    while (1)
    {
        c = fgetc(stdin);
        switch (state)
        {
            case START:
            if (isspace(c)) {
                state = START;
            }
            else if (c == '\n') {
                return EOL;
            }
            else if (c=='/') {
                state = COMMENT_OPERATOR;
            }
            else if (c=='+') {
                return (PLUS);
            }
            else if (c=='-') {
                return (MINUS);
            }
            else if (c=='*') {
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
                return (L_BRACKET);
            }
            else if (c==')') {
                return (R_BRACKET);
            }
            else if (c=='{') {
                return (L_BRACKET1);
            }
            else if (c=='}') {
                return (R_BRACKET1);
            }
            else if (c==',') {
                return (COMA);
            }
            else if (c==';') {
                return (UNDERSCORE);
            }
            else if (c=='_') {
                state = ID;
                strAddChar(strtmp,c);
            }
            else if (isalpha(c)) {
                state = ID_KW;
                strAddChar(strtmp,c);
            }

            else if (c=='"') {
                state = STRING;
            }
            else if (isdigit(c)) {
                state = NUMBER;
                strAddChar(strtmp,c);
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
                    if (strCmpConstStr(strtmp,"else")) {
                        return (ELSE);
                    }
                    else if (strCmpConstStr(strtmp,"float64")) {
                        return (FLOAT64);
                    }
                    else if (strCmpConstStr(strtmp,"for")) {
                        return (FOR);
                    }
                    else if (strCmpConstStr(strtmp,"func")) {
                        return (FUNC);
                    }
                    else if (strCmpConstStr(strtmp,"if")) {
                        return (IF);
                    }
                    else if (strCmpConstStr(strtmp,"int")) {
                        return (PACKAGE);
                    }
                    else if (strCmpConstStr(strtmp,"return")) {
                        return (RETURN);
                    }
                    else if (strCmpConstStr(strtmp,"string")) {
                        return (STRING);
                    }
                    else {
                        return (ID);
                    }
                }
                break;

            case ID:
                if (c == '_' || isalnum(c)) {
                    strAddChar(strtmp,c);
                }
                else {
                    ungetc(c,stdin);
                    return (ID);
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
                    return INT;
                }
            break;

            case FLOAT:
                if (isdigit(c)) {
                    strAddChar(strtmp,c);
                }
                else if (c == 'E' || c == 'e') {
                    strAddChar(strtmp, c);
                    state = EXPONENT_SIGN;
                }
                else {
                    ungetc(c,stdin);
                    return (FLOAT);
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
                    return (LEX_ERROR);
                }
             break;

            case EXPONENT_CHECK:
                if (isdigit(c)) {
                    strAddChar(strtmp,c);
                    state = EXPONENT;
                }
                else {
                    return (LEX_ERROR);
                }
            break;

            case EXPONENT:
                if (isdigit(c)) {
                    strAddChar(strtmp, c);
                }
                else {
                    ungetc (c,stdin);
                    return (FLOAT);
                }
            break;


            case STRING:
                if (c=='"') {
                    return (STRING_LIT);
                }
                else if (c=='\\') {
                    state = ESCAPE_SEQ;
                }
                else if (c>31) {
                    strAddChar(strtmp,c);
                }
                else  {
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
                    return (LEX_ERROR);
                }
                break;

            case STRING_HEX:
                if (i<2 && isdigit(c)) {
                    hexc[i]=c;
                    i++;

                }
                else if (i==0) {
                    return (LEX_ERROR);
                }
                else {
                    hexc[i] = '\0';
                    hexo = strtol(hexc,NULL,16);
                    if (hexo == 0) {
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
                    return (EQUAL);
                }
                else {
                    ungetc(c,stdin);
                    return (ASSIGN);
                }

            case OPERATOR:
                strAddChar(strtmp,c);
                if (strCmpConstStr(strtmp,"<=")) {
                    return (LESSEQUAL);
                }
                else if (strCmpConstStr(strtmp,">=")) {
                    return (GREATEQUAL);
                }
                else if (strCmpConstStr(strtmp,"!=")) {
                    return (NOTEQUAL);
                }
                else {
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
                state = BLOCK_COMMENT;

            }
            break;

            default:
                break;


        }
        }

        }
        int main() {
            int k;
            k = get_token();
            printf("%d",k);
            return (0);
        }
