#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "codegen.h"
#include "scanner.h"
#include "global.h"

#define assembler 2


//zaciatok generovania
void codegen_start () {
    #ifdef assembler
    printf (".IFJcode20");
    printf ("\nDEFVAR GF@*temp1");
    printf ("\nDEFVAR GF@*temp2");
    printf ("\nDEFVAR GF@*temp3");
    printf ("\nDEFVAR GF@*booltemp");
    printf ("\nJUMP $main");
    #endif
}

//main
void codegen_main () {
    #ifdef assembler
    printf ("\n\nLABEL $main");
    printf ("\nCREATEFRAME");
    printf ("\nPUSHFRAME");
    #endif
}


//vygenerovanie label funkciu
void codegen_funclabel(char * label){
    #ifdef assembler
    printf("\n\nLABEL $%s", label);
    printf("\nPUSHFRAME");
    #endif
}


//navratova hodnota funkcie do prislusnej premennej
void codegen_func_param(int order, char * name){
    #ifdef assembler
    printf("\n DEFVAR LF@%s", name);
    printf("\nMOVE LF@%s LF@*param%d", name, order);
    #endif
}

//koniec funkcie
void codegen_func_end(){
    #ifdef assembler
    printf ("\nPOPFRAME");
    printf ("\nRETURN");
    #endif
}


//return z funkcie
void codegen_func_return(int order){
    #ifdef assembler
    printf("\nDEFVAR LF@*retval%d", order);
    printf("\nPOPS LF@*retval%d", order );
    #endif
}

//osetrenie defvaru premennej vo fore
void codegen_for_defvar(int label, char * id){
    printf("\nEQ GF@*booltemp LF@*forcounter%d int@0", label);
    printf("\nJUMPIFEQ $*defvarlabel%s GF@*booltemp bool@true", id);
    printf("\nPOPS LF@%s", id);
    printf("\nJUMP $*defvarlabel%send", id);
    printf("\nLABEL $*defvarlabel%s", id);
    printf("\nDEFVAR LF@%s", id);
    printf("\nPOPS LF@%s", id);
    printf("\nLABEL $*defvarlabel%send", id);

}

//zaciatok foru
void codegen_for_start(int label){
    #ifdef assembler
    printf ("\nDEFVAR LF@*tempfor%d", label);
    printf("\nDEFVAR LF@*forcounter%d", label);
    printf("\nMOVE LF@*forcounter%d int@0", label);
    printf("\nLABEL $for_loop_check%d", label);   
    #endif
}


//bool vyraz vo fore
void codedegen_for_expr(int label){
     #ifdef assembler
    printf("\nPOPS LF@*tempfor%d", label);
    printf("\nJUMPIFEQ $forendlabel%d LF@*tempfor%d bool@false", label, label);
    #endif
}


//priradenie vo fore
void codegen_for_assign_start(int label){
    #ifdef assembler
    printf("\nJUMP $for_assign_end%d", label);
    printf("\nLABEL $for_assign_start%d", label);
    #endif
}


//koniec priradenia
void codegen_for_assign_end(int label){
    #ifdef assembler
    printf("\nJUMP $for_return_assign%d", label);
    printf("\nLABEL $for_assign_end%d", label);
    #endif
}

//for - cyklus
void codegen_for_cycle(int label){
    #ifdef assembler
    printf("\nLABEL $for_cycle_start%d", label);
    #endif
}

//vyhodnotenie pokracovania cyklu
void codegen_for_check(int label){
    #ifdef assembler
    printf("\nJUMP $for_assign_start%d", label);
    printf("\nLABEL $for_return_assign%d", label);
    printf("\nADD LF@*forcounter%d LF@*forcounter%d int@1", label, label);
    printf("\nJUMP $for_loop_check%d", label);
    #endif
}

//koniec foru
void codegen_for_end(int label){
    #ifdef assembler
     printf("\nLABEL $forendlabel%d", label);
    #endif
}

//zaciatok ifu
void codegen_if_start(int label){
    #ifdef assembler

    printf ("\nDEFVAR LF@*tempif%d", label);
    printf("\nPOPS LF@*tempif%d", label);
    printf("\nJUMPIFEQ $elselabel_%d LF@*tempif%d bool@false", label, label);
    #endif
}


//preskocenie else
void codegen_else_skip(int label){
    #ifdef assembler
    printf ("\nJUMP $endiflabel_%d", label);
    #endif
}

//else label
void codegen_else_label(int label){
    #ifdef assembler
    printf("\nLABEL $elselabel_%d", label);
    #endif
}


//koniec if vetvy
void codegen_if_end(int label){
    #ifdef assembler
    printf("\nLABEL $endiflabel_%d", label);
    #endif
}

//numbericka operacia
void codegen_operation(int operation) {
    #ifdef assembler
    switch(operation)
    {
        case PLUS:
        printf ("\nADDS");
        break;
        case MINUS:
        printf ("\nSUBS");
        break;
        case MULTIPLICATION:
        printf ("\nMULS");
        break;
        case DIV:
        printf ("\nDIVS");
        break;
        case IDIV:
        printf ("\nIDIVS");
        break;
        case GREAT:
        printf ("\nGTS");
        break;
        case LESS:
        printf ("\nLTS");
        break;
        case EQUAL:
        printf ("\nEQS");
        break;
        case NOTEQUAL:
        printf ("\nEQS");
        printf ("\nNOTS");
        break;
        case LESSEQUAL:
        printf ("\nPOPS GF@*temp1");
        printf ("\nPOPS GF@*temp2");
        printf ("\nPUSHS GF@*temp2");
        printf ("\nPUSHS GF@*temp1");
        printf ("\nLTS");
        printf ("\nPUSHS GF@*temp2");
        printf ("\nPUSHS GF@*temp1");
        printf ("\nEQS");
        printf ("\nORS");
        break;
        case GREATEQUAL:
        printf ("\nPOPS GF@*temp1");
        printf ("\nPOPS GF@*temp2");
        printf ("\nPUSHS GF@*temp2");
        printf ("\nPUSHS GF@*temp1");
        printf ("\nGTS");
        printf ("\nPUSHS GF@*temp2");
        printf ("\nPUSHS GF@*temp1");
        printf ("\nEQS");
        printf ("\nORS");
        break;
        case CONCAT:
        printf ("\nPOPS GF@*temp1");
        printf ("\nPOPS GF@*temp2");
        printf ("\nCONCAT GF@*temp3 GF@*temp2 GF@*temp1");
        printf ("\nPUSHS GF@*temp3");
        break;

        default:
        break;
    }  
    #endif   
}


//volanie funkcie
void codegen_call(char* func){
    #ifdef assembler
    printf ("\n CALL $%s",func);
    #endif
}

//create frame
void codegen_createframe(){
    #ifdef assembler
    printf("\nCREATEFRAME");
    #endif
}


//built in inputs
void codegen_call_inputs(){
    #ifdef assembler
    printf("\nCREATEFRAME");
    printf("\nCALL $inputs");
    #endif
}


//vlozenie navratovej hodnoty do premennej
void codegen_funcreturn(char * return_id, int order){
    #ifdef assembler
    if (return_id != NULL){
        printf("\nMOVE LF@%s TF@*retval%d", return_id, order);
    }
    #endif
}

//built-in subst()
void codegen_substr(){
    #ifdef assembler
    printf ("\n\nLABEL $substr");
    printf("\nPUSHFRAME");
    printf ("\nDEFVAR LF@*retval1"); //vysledny string 
    printf ("\nDEFVAR LF@*retval2"); //error hodnota
    printf ("\nDEFVAR LF@*string"); 
    printf ("\nDEFVAR LF@*position"); 
    printf ("\nDEFVAR LF@*length"); //dlzka retazca
    printf ("\nMOVE LF@*string LF@*param1"); //parameter 1
    printf ("\nMOVE LF@*position LF@*param2"); //parameter2 
    printf ("\nMOVE LF@*length LF@*param3"); //parameter3
    printf ("\n MOVE LF@*retval1 string@"); //prazdny string
    printf ("\n MOVE LF@*retval2 int@0"); 

    printf ("\n DEFVAR LF@*len"); //premenna, do ktorej sa ulozi len
    //zavolanie len()
    printf("\nCREATEFRAME"); 
    printf("\nDEFVAR TF@*param1");
    printf("\nMOVE TF@*param1 LF@*string");
    printf("\nCALL $len");   
    printf("\nMOVE LF@*len TF@*retval1");
    printf("\nSUB LF@*len LF@*len int@1"); //odcitanie 1

    printf("\nDEFVAR LF@*temp");
    //kontola dolnej medze - 0 pre position
    printf("\nLT LF@*temp LF@*position int@0");
    printf("\nJUMPIFNEQ $error_substr LF@*temp bool@false");

    //kontrola hornej medze - len pre position
    printf("\nGT LF@*temp LF@*position LF@*len");
    printf("\nJUMPIFNEQ $error_substr LF@*temp bool@false");

    //kontola dolnej medze - 0 pre length
    printf("\nLT LF@*temp LF@*length int@0");
    printf("\nJUMPIFNEQ $error_substr LF@*temp bool@false");

    printf("\nDEFVAR LF@*tempchar"); //bude sa nacitat char
    printf("\nDEFVAR LF@*counter"); //counter pre loop
    printf("\nDEFVAR LF@*actposition"); //aktualna pozicia v ramci stringu 
    printf ("\n MOVE LF@*counter int@0"); //vynulovanie counteru
    printf ("\n MOVE LF@*actposition LF@*position"); 

    
    printf ("\nLABEL $substr_loop");
    printf("\nEQ LF@*temp LF@*counter LF@*length");
    printf("\nJUMPIFNEQ $end_substr LF@*temp bool@false"); //pokial counter = length
    printf("\nGT LF@*temp LF@*actposition LF@*len");
    printf("\nJUMPIFNEQ $end_substr LF@*temp bool@false"); //pokial actposition = len()-1
    printf ("\nGETCHAR LF@*tempchar LF@*string LF@*actposition");
    printf("\nCONCAT LF@*retval1 LF@*retval1 LF@*tempchar");


    printf("\nADD LF@*counter LF@*counter int@1"); //inkrementovanie o 1
    printf("\nADD LF@*actposition LF@*actposition int@1"); //inkrementovanie o 1
    printf ("\nJUMP $substr_loop");

    //zle
    printf ("\nLABEL $error_substr");
    printf ("\nMOVE LF@*retval1 string@error");
    printf ("\n MOVE LF@*retval2 int@1");

    printf ("\nLABEL $end_substr");
    printf ("\nPOPFRAME");
    printf ("\nRETURN");

    #endif 
}

//built-in ord()
void codegen_ord(){
    #ifdef assembler
    printf ("\n\nLABEL $ord");
    printf("\nPUSHFRAME");
    printf ("\nDEFVAR LF@*retval1"); //ord hodnota
    printf ("\n DEFVAR LF@*retval2"); //error hodnota
    printf ("\n DEFVAR LF@*string"); 
    printf ("\n DEFVAR LF@*position"); 
    printf ("\nMOVE LF@*string LF@*param1"); //parameter 1
    printf ("\nMOVE LF@*position LF@*param2"); //parameter2
    printf ("\n MOVE LF@*retval2 int@0"); 


    printf ("\n DEFVAR LF@*len"); //premenna, do ktorej sa ulozi len
    //zavolanie len()
    printf("\nCREATEFRAME"); 
    printf("\nDEFVAR TF@*param1");
    printf("\nMOVE TF@*param1 LF@*string");
    printf("\nCALL $len");   
    printf("\nMOVE LF@*len TF@*retval1");
    printf("\nSUB LF@*len LF@*len int@1"); //odcitanie 1

    printf("\nDEFVAR LF@*temp");

    //kontola dolnej medze - 0
    printf("\nLT LF@*temp LF@*position int@0");
    printf("\nJUMPIFNEQ $error_ord LF@*temp bool@false");

    //kontrola hornej medze - len
    printf("\nGT LF@*temp LF@*position LF@*len");
    printf("\nJUMPIFNEQ $error_ord LF@*temp bool@false");

    printf ("\nSTRI2INT LF@*retval1 LF@*string LF@*position");
    printf ("\nJUMP $end_ord");
    printf ("\nLABEL $error_ord");
    printf ("\nMOVE LF@*retval1 int@0");
    printf ("\n MOVE LF@*retval2 int@1");
    printf ("\nLABEL $end_ord");
    printf ("\nPOPFRAME");
    printf ("\nRETURN");
        


    #endif
}


//built-in chr()
void codegen_chr() {
    #ifdef assembler
    printf ("\n\nLABEL $chr");
    printf ("\nPUSHFRAME");
    printf ("\nDEFVAR LF@*retval1");
    printf ("\nDEFVAR LF@*retval2");
    printf ("\nMOVE LF@*retval2 int@0");
    printf ("\nDEFVAR LF@*position");
    printf ("\nMOVE LF@*position LF@*param1"); //parameter
    printf ("\nDEFVAR LF@*temp"); //vysledok porovnavania
    //kontola dolnej medze - 0
    printf("\nLT LF@*temp LF@*position int@0");
    printf("\nJUMPIFNEQ $error_chr LF@*temp bool@false");

    //kontrola hornej medze - 255
    printf("\nGT LF@*temp LF@*position int@255");
    printf("\nJUMPIFNEQ $error_chr LF@*temp bool@false");
    //spravne
    printf ("\nINT2CHAR LF@*retval1 LF@*param1");
    printf ("\nJUMP $end_chr");
    printf ("\nLABEL $error_chr");
    printf ("\nMOVE LF@*retval1 string@error");
    printf ("\n MOVE LF@*retval2 int@1");
    printf ("\nLABEL $end_chr");
    printf ("\nPOPFRAME");
    printf ("\nRETURN");
    #endif
}


//built-in len()
void codegen_len() {
    #ifdef assembler
    printf ("\n\nLABEL $len");
    printf ("\nPUSHFRAME");
    printf ("\nDEFVAR LF@*temp");
    printf ("\nDEFVAR LF@*retval1");
    printf ("\nMOVE LF@*temp LF@*param1"); //prvy parameter
    printf ("\nSTRLEN LF@*retval1 LF@*temp");
    printf ("\nPOPFRAME");
    printf ("\nRETURN");
    #endif
}

//built-in int2float()
void codegen_int2float() {
    #ifdef assembler
    printf ("\n\nLABEL $int2float");
    printf ("\nPUSHFRAME");
    printf ("\nDEFVAR LF@*retval1");
    printf ("\nINT2FLOAT LF@*retval1 LF@*param1");
    printf ("\nPOPFRAME");
    printf ("\nRETURN");
    #endif
}


//built-in float2int
void codegen_float2int() {
    #ifdef assembler
    printf ("\n\nLABEL $float2int");
    printf ("\nPUSHFRAME");
    printf ("\nDEFVAR LF@*retval1");
    printf ("\nFLOAT2INT LF@*retval1 LF@*param1");
    printf ("\nPOPFRAME");
    printf ("\nRETURN");
    #endif
}



//built-in inputs()
void codegen_inputs() {
    #ifdef assembler
    printf ("\n\nLABEL $inputs");
    printf ("\nPUSHFRAME");
    printf ("\nDEFVAR LF@*retval1");
    printf ("\n DEFVAR LF@*retval2");
    printf ("\n MOVE LF@*retval2 int@0");
    printf ("\nREAD LF@*retval1 string");
    printf ("\nJUMPIFEQ $error_inputs LF@*retval1 nil@nil");
    printf ("\nJUMP $end_inputs");
    printf ("\nLABEL $error_inputs");
    printf ("\n MOVE LF@*retval2 int@1");
    printf ("\nLABEL $end_inputs");
    printf ("\nPOPFRAME");
    printf ("\nRETURN");
    #endif
}

/*void codegen_inputi() {
    #ifdef assembler
    printf ("\n\nLABEL $inputi");
    printf ("\nPUSHFRAME");
    printf ("\nDEFVAR LF@*retval1");
    printf ("\n DEFVAR LF@*retval2");
    printf ("\n MOVE LF@*retval2 int@0");
    printf ("\nREAD LF@*retval1 int");
    printf ("\nJUMPIFEQ $error_inputi LF@*retval1 int@198");
    printf ("\nJUMP $end_inputi");
    printf ("\nLABEL $error_inputi");
    //printf("\nWRITE string@tu");
    printf ("\n MOVE LF@*retval2 int@1");
    printf ("\nLABEL $end_inputi");
    printf ("\nPOPFRAME");
    printf ("\nRETURN");
    #endif
}*/

//built-in inputi()
void codegen_inputi() {
    #ifdef assembler
    printf ("\n\nLABEL $inputi");
    printf ("\nPUSHFRAME");
    printf ("\nDEFVAR LF@*retval1");
    printf ("\n DEFVAR LF@*retval2");
    printf ("\n MOVE LF@*retval2 int@0");
    printf ("\nREAD LF@*retval1 int");
    printf ("\nJUMPIFEQ $error_inputi LF@*retval1 nil@nil");
    printf ("\nJUMP $end_inputi");
    printf ("\nLABEL $error_inputi");
    //printf("\nWRITE string@tu");
    printf ("\n MOVE LF@*retval2 int@1");
    printf ("\nLABEL $end_inputi");
    printf ("\nPOPFRAME");
    printf ("\nRETURN");
    #endif
}



//built-in inputf()
void codegen_inputf() {
    #ifdef assembler
    printf ("\n\nLABEL $inputf");
    printf ("\nPUSHFRAME");
    printf ("\nDEFVAR LF@*retval1");
    printf ("\n DEFVAR LF@*retval2");
    printf ("\n MOVE LF@*retval2 int@0");
    printf ("\nREAD LF@*retval1 float");
    printf ("\nJUMPIFEQ $error_inputf LF@*retval1 nil@nil");
    printf ("\nJUMP $end_inputf");
    printf ("\nLABEL $error_inputf");
    printf ("\n MOVE LF@*retval2 int@1");
    printf ("\nLABEL $end_inputf");
    printf ("\nPOPFRAME");
    printf ("\nRETURN");
    #endif
}

//vsunutie hodnoty na zasobnik
void codegen_push(int type, char* value) {
    #ifdef assembler
    printf("\nPUSHS");
    switch(type)
    {
        case IDENTIFIER:
        printf (" LF@%s",value);
        break;
        case INT:
        printf (" int@%s",value);
        break;
        case FLOAT_N:
        case FLOAT_L:
        printf (" float@%a",strtof(value, NULL));
        break;
        case STRING_LIT:
        printf (" string@%s",value);
        break;
        default:
        break;
  }
    #endif 
}


//priradenie parametru do funkcie
void codegen_param(int type, char * value, int order){
    #ifdef assembler
    printf("\nDEFVAR TF@*param%d", order);
    printf("\nMOVE TF@*param%d", order);
    switch(type){
        case IDENTIFIER:
        printf (" LF@%s",value);
        break;
        case INT:
        printf (" int@%s",value);
        break;
        case FLOAT_N:
        case FLOAT_L:
        printf (" float@%a",strtof(value, NULL));
        break;
        case STRING_LIT:
        printf (" string@%s",value);
        break;
        default:
        break;

    }
    #endif
}

//deklarovanie premennej
void codegen_declare(char * id){
    #ifdef assembler
    printf("\nDEFVAR LF@%s", id);
    printf("\nPOPS LF@%s", id);
    #endif
}

//priradenie hodnoty premennej
void codegen_assign(char * id){
    #ifdef assembler
    printf("\nPOPS LF@%s", id);
    #endif
}



//built-in print()
void codegen_write (int type, char* value) {
    #ifdef assembler
    switch (type)
    {
        case INT:
        printf ("\nWRITE int@%s",value);
        break;
        case FLOAT_N:
        case FLOAT_L:
        printf ("\nWRITE float@%a", strtof(value, NULL));
        break;
        case STRING_LIT:
        printf ("\nWRITE string@%s",value);
        break;
        case IDENTIFIER:
        printf ("\nWRITE LF@%s",value);
        break;
        default:
        break;
    }
    #endif
}

//koniec generovania
void codegen_end(){
    #ifdef assembler

    printf ("\nPOPFRAME");
    printf ("\nCLEARS");
    printf("\nEXIT int@0");
    
    #endif
}