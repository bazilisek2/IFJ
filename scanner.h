/****
 * IFJ Projekt 2020
 * 
 * Autori:
 * xzimme03 - Marián Zimmermann 
 * 
 * Súhrn: Lexikálny analyzátor (scanner) 
 * Prechádza v cykle while znaky a vracia ich ako tokeny ktoré sa rozlíšia vo switche 
 * 
 * 
 * */

#ifndef SCANNER_H
#define SCANNER_H

#include "str.h"

#define FLOAT_N 4 //cislo float
#define GREAT 8
#define LESS 9
#define PLUS 10
#define MINUS 11
#define MULTIPLICATION 12
#define EQUAL 13
#define ASSIGN 14
#define DIVISION 15
#define LESSEQUAL 16
#define GREATEQUAL 17
#define NOTEQUAL 18
#define DEFINITION 19
#define L_BRACKET 21
#define R_BRACKET 22
#define L_BRACKET1 23
#define R_BRACKET1 24
#define COMA 25
#define UNDERSCORE 26 //;
#define UNDERLINE 27 // _
#define ELSE 30
#define FLOAT64 31 //type float
#define FOR 32
#define FUNC 33
#define IF 34
#define PACKAGE 35
#define RETURN 36
#define INT 40
#define FLOAT_L 41
#define STRING_LIT 42 //"string"
#define STRING_T 43 //type string
#define INT_T 44 // type int
#define EOL 50
#define LEX_ERROR 99
#define IDENTIFIER 60
#define END_OF_FILE 61
#define PRINT 70
#define INT2FLOAT 71
#define LEN 72
#define SUBSTR 73
#define ORD 74
#define INPUTS 75
#define INPUTI 76
#define INPUTF 77
#define FLOAT2INT 78

int find_eol();
int get_token(int mode, int * flag, string *strtmp);

#endif
