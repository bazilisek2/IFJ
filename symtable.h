#ifndef SYMTABLE_H
#define SYMTABLE_H
#define TRUE 1
#define FALSE 0

typedef struct symtable {
	char Key;			                                                   
	void* Data;                                            
	struct symtable* LPtr;                                  
	struct symtable* RPtr;                                  
} *symtablePtr;

void TInit(symtablePtr*);
int TSearch(symtablePtr, char, void*);
void TInsert(symtablePtr*, char, void);
void TDelete(symtablePtr*, char);
void TDispose(symtablePtr*);

#endif 
