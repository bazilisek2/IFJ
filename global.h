#ifndef GLOBAL_H
#define GLOBAL_H

//miesto bool
#define TRUE 1
#define FALSE 0

//flag pre scanner
#define NO_EOL 0
#define EOL_FOUND 1

//mody pre scanner
#define GET 0
#define PEEK 1

//buffer v scaneri 
#define EMPTY -12
#define NOT_KNOWN -13

//Errory
#define LEX_ERROR 99 //chyba 1
#define SYN_ERROR 100 //chyba 2
#define SEM_ERROR 101 //chyba 3
#define PARAM_ERROR 102 //chyba 6
#define TYPE_ERROR 103 //chyba 5

//zapnutie/vypnutie DEBUG vypisov
#define DEBUG_ACTIVE 1

#define DEBUG_MODE(msg); if(DEBUG_ACTIVE) printf("%s\n", msg);


#endif