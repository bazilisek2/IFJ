#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include "scanner.h"

//ErrorCodes
#define OK 0                // překlad proběhne bez chyb
#define ERR_SCANNER 1       // chyba v programu v rámci lexikální analýzy
#define ERR_PARSER 2        // chyba v programu v rámci syntaktické analýzyy
#define ERR_NODEF 3         // nedefinovaná funkce/proměnná, pokus o redefinici funkce/proměnné 
#define ERR_TYPE 4          // sémantická chyba při odvozování datového typu nově definované proměnné
#define ERR_COM 5           // chyba typové kompatibility v aritmetických, řetězcových a relačních výrazech
#define ERR_FUNC 6          // špatný počet/typ parametrů či návratových hodnot u volání funkce či návratu z funkce
#define ERR_SEMANTIC 7      // ostatní sémantické chyby      
#define ERR_ZERODIV 9       // chyba dělení nulovou konstantou     
#define ERR_INTERPRET 99    // chyba překladače

typedef struct
{
    char* str;		// místo pro daný řetězec ukončený '\0'
    int length;		// délka řetězce
    int allocSize;	// velikost alokované paměti
} string;

int strInit(string* s);
void strFree(string* s);
void strClear(string* s);
int strAddChar(string* s1, char c);
int strCmpConstStr(string* s1, char* s2);
