#ifndef SCANNER_H
#define SCANNER_H


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
#define FLOAT_L 41
#define STRING_LIT 42
#define EOL 50
#define LEX_ERROR 99
#define IDENTIFIER 60
#define END_OF_FILE 61
int get_token();

#endif