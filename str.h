/******** str.h *********
 * 
 * Implementace překladače imperativního jazyka IFJ20.
 *
 * Authors: Adrián Matušík, xmatus35
 *          Marián Zimmermann, xzimme03
 *
 */
 
#ifndef STR_INCLUDED
#define STR_INCLUDED
  typedef struct
  {
    char* str;		  // miesto pre daný retazec ukončený '\0'
    int length;		  // dlzka retazca
    int allocSize;	// velkost alokovanej pamate
  } string;

  int strInit(string* s);
  void strFree(string* s);
  void strClear(string* s);
  int strAddChar(string* s1, char c);
  int strCmpConstStr(string* s1, char* s2);
#endif
