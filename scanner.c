/******** scanner.c *********
 *
 * Implementace překladače imperativního jazyka IFJ20.
 *
 * Authors: Adrián Matušík, xmatus35
 *          Marián Zimmermann, xzimme03
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include "str.h"
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
