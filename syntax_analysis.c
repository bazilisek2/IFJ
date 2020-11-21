#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "scanner.h"
#include "global.h"
#include "syntax_analysis.h"

int retvalue; //kód chyby 
int EOL_flag = 0;
string atr;


int is_term(int token){
    if (token == IDENTIFIER || token == INT || token == FLOAT_N || token == STRING_LIT || token == FLOAT_L){
        return TRUE;
    }
    
    return FALSE;
}

//typ termu - string, float, lit
int is_type(int token){
    if (token == INT_T || token == FLOAT64 || token == STRING_T){
        return TRUE;
    }
    
    return FALSE;
}

int analysis_start();
int prolog();
int func();
int args();
int return_params();
int s_body();
int s_if();
int s_return();
int f_params();
int f_definition();
int f_assign();
int expr();
int s_for();

/*****
 * Parametre pre vsetky podporovane typy
 * 
 * */
 
int analysis_start(){
    strInit(&atr);
    int token = get_token(PEEK, &EOL_flag, &atr);

    //prazdny subor
    if (token == END_OF_FILE){ 
 
        DEBUG_MODE("Prazdny subor")   
        return TRUE; //MOZE BYT SUBOR PRAZDNY?

    }  

    if ((retvalue = prolog()) != TRUE){
        DEBUG_MODE("Zly prolog")
        return retvalue;
    }

    DEBUG_MODE("Prolog dobry")
        
    //package main
    while (token != END_OF_FILE){
        token = get_token(GET, &EOL_flag, &atr);
        if (token == LEX_ERROR){
            return LEX_ERROR;
        }

        /****package main

        funkcie

        ******************/
      
        if (token == FUNC && EOL_flag == EOL_FOUND){ //pred func, za package main EOL
            if ((retvalue = func()) != TRUE) {
                //syntakticka alebo lexikalna chyba vo funkcii
                return retvalue;
            }
        }

        else if (token == END_OF_FILE){
            DEBUG_MODE("Koniec suboru")
            break;
        }

        //cokolvek, co nezacina tokenom "func"
        else {
            DEBUG_MODE("Zla struktura programu - nie je sekvencia funkcii")
            return FALSE;
        }

    }
                       
    return TRUE;
}

int func(){
    int token;
    
    //func ID
    token = get_token(GET, &EOL_flag, &atr);
    if (token == LEX_ERROR){
        return LEX_ERROR;
    }
    if (token!= IDENTIFIER || EOL_flag != NO_EOL) {
        DEBUG_MODE("Zle ID")
        return SYN_ERROR;
    }



    //func ID (
    token = get_token(GET, &EOL_flag, &atr);
    if (token == LEX_ERROR){
        return LEX_ERROR;
    }

    if (token != L_BRACKET || EOL_flag != NO_EOL){
        DEBUG_MODE("Chyba vo funkcii v zatvorke pred argumentmi")
        return SYN_ERROR;
    }

    //pozriem, co bude dalsie, ale nenacitam
    token = get_token(PEEK, &EOL_flag, &atr);

    switch (token){
        
        case IDENTIFIER:
            //func ID (string...
            if ((retvalue = args()) != TRUE){
                DEBUG_MODE("Zle argumenty funkcie!")
                return retvalue;
            }
            break;
        case R_BRACKET:
            token = get_token(GET, &EOL_flag, &atr); //realne nacitanie zatvorky
            
            if (EOL_flag != NO_EOL){
                return SYN_ERROR;
            }
            //func ID ( )
            break;

        default:

        if (token == LEX_ERROR){
            return LEX_ERROR;
        }
            //func ID ( chyba
            DEBUG_MODE("Neocakavany znak v argumentoch")
            return SYN_ERROR;
    }

    //func ID (args)...
    token = get_token(GET, &EOL_flag, &atr);
    if (token == LEX_ERROR){
        return LEX_ERROR;
    }
    if ((token == L_BRACKET1) && (EOL_flag == NO_EOL)){
        
        //func id () {....
        DEBUG_MODE("Return zatvorka skipnuta")
        if ((retvalue = s_body()) != TRUE){
            DEBUG_MODE("Zle telo funkcie")
            return retvalue;
        }

    }
    else if (token == L_BRACKET){
         
         //func ID (args) (...
         token = get_token(PEEK, &EOL_flag, &atr);

         if (token == LEX_ERROR){
             return LEX_ERROR;
         }

         //func ID (args) (type...
        if ((is_type(token)) && EOL_flag == NO_EOL){
             if ((retvalue = return_params()) != TRUE){
                 DEBUG_MODE("Zle return parametre")
                 return retvalue;
             }

              //func ID (args) (return_params) {
             if ((token = get_token(GET, &EOL_flag, &atr)) == L_BRACKET1){
                if ((retvalue = s_body()) != TRUE){
                    DEBUG_MODE("Zle telo funkcie")
                    return retvalue;
                }
             }
             else {
                 DEBUG_MODE("Neocakavany znak miesto {")
             }
             
         }
         else {
             DEBUG_MODE("Zle pokracovanie return zatvorky")
             return SYN_ERROR;
         }
         
    }

    /*func ID (args) (return_params) {
        body
       }*/
    
    //celá funkcia ok
    return TRUE;
    
}

int s_body(){
    int token;
    int id_counter = 0; //pocet id a <expr> musi sediet

    //koniec tela = }
    while (token != R_BRACKET1){
        int is_underline;
        token = get_token(GET, &EOL_flag, &atr);
        if (token == LEX_ERROR){
            return LEX_ERROR;
        }

        switch(token){
        
        //id
        case IDENTIFIER:
        case UNDERLINE: 

            if (EOL_flag != EOL_FOUND){
                return SYN_ERROR;
            }

            if (token == UNDERLINE){
                is_underline = TRUE;
            }

            token = get_token(GET, &EOL_flag, &atr);
            if (token == LEX_ERROR){
                return LEX_ERROR;
            }
            switch (token) {
                
                //id := 
                case DEFINITION:
                    
                    DEBUG_MODE("STATEMENT DEFINITION")

                    if (EOL_flag != NO_EOL){
                        return SYN_ERROR;
                    }

                    if(is_underline == TRUE){ //veľmi nahardcodené to, že _ nemôže byť začiatkom definície
                        DEBUG_MODE("Definovanie do prazdneho id")
                        return SYN_ERROR;
                    }

                    //id := <expr>
                    if ((retvalue = expr()) != TRUE){
                        DEBUG_MODE("Chyba v definici")
                        return retvalue;
                    }
                    break;
                
                //id = jednoduché priradenie alebo priradenie funkcie
                case ASSIGN: 


                    DEBUG_MODE("STATEMENT SINGLE ASSIGN")
                    if (EOL_flag != NO_EOL){
                        return SYN_ERROR;
                    }

                    token = get_token(PEEK, &EOL_flag, &atr);
                    if (token == LEX_ERROR){
                        return LEX_ERROR;
                        }
                    
                    //id = func()
                    DEBUG_MODE("STATEMENT ASSIGN FUNCTION")
                    if (token == IDENTIFIER || EOL_flag != NO_EOL){
                        DEBUG_MODE("Jednoduche priradenie do funkcie")

                        token = get_token(GET, &EOL_flag, &atr); //realne nacitam ID

                        token = get_token(GET, &EOL_flag, &atr);
                        if (token == LEX_ERROR){
                            return LEX_ERROR;
                        }

                        if ( token != L_BRACKET || EOL_flag != NO_EOL) {
                            DEBUG_MODE("Chyba ( za funkciou za jednoduchym zavolanim fukncie")
                            return SYN_ERROR;
                            } 

                        if ((retvalue = f_params()) != TRUE){
                            DEBUG_MODE("Zle parametre funkcie za jednoduchym zavolanim funkcie")
                            return retvalue;
                            }

                    }
                    else {

                        //id = <expr>
                        if ((retvalue = expr()) != TRUE){
                            DEBUG_MODE("Chyba v jednoduchom priradeni v <expr>")
                            return retvalue;
                        }
                        
                    }

                    break;

                //id( 
                case L_BRACKET: 
                    if (EOL_flag != NO_EOL){
                        return SYN_ERROR;
                    }
                    // id (
                    if ((retvalue = f_params()) != TRUE){
                            DEBUG_MODE("Zle parametre funkcie za volanim fukncie")
                            return retvalue;
                            }

                    DEBUG_MODE("id (params)")
                    break;

                //id, 
                case COMA:
                        
                        //jedno ID uz je nacitane
                        id_counter = 1;
                        while(1){

                            token = get_token(GET, &EOL_flag, &atr);
                            if (token == LEX_ERROR){
                                return LEX_ERROR;
                            }
                            
                            //id, id
                            if (token == IDENTIFIER || token == UNDERLINE){
                                if (EOL_flag != NO_EOL){
                                    return SYN_ERROR;
                                }

                                id_counter++;

                                token = get_token(GET, &EOL_flag, &atr);
                                if (token == LEX_ERROR){
                                    return LEX_ERROR;
                                }
                                
                                //id, id = 
                                if ((token == ASSIGN) && (EOL_flag == NO_EOL)){
                                    break;
                                }

                                //id, id, 
                                else if (token != COMA || EOL_flag != NO_EOL){
                                    DEBUG_MODE("Za id, id... nieco zle")
                                    return SYN_ERROR;
                                }
                            }
                            
                            else {
                                DEBUG_MODE("Za id, sa nacitalo nieco blbe")
                                return SYN_ERROR;
                            }
                        }
                        //id, id.... = 
                        token = get_token(PEEK, &EOL_flag, &atr);
                        if (token == LEX_ERROR){
                            return LEX_ERROR;
                        }
                        
                        //id, id... = ID 
                        if ((token == IDENTIFIER) && EOL_flag == NO_EOL){
                            
                            
                            token = get_token(GET, &EOL_flag, &atr); //skutocne nacitanie func id
                            if (token == LEX_ERROR){
                                return LEX_ERROR;
                            }
                            DEBUG_MODE("Precitana funkcia za viacnasobnym priradenim")
                            
                            //id, id... = ID (
                            token = get_token(GET, &EOL_flag, &atr);
                            if (token != L_BRACKET || EOL_flag != NO_EOL) {
                                DEBUG_MODE("Chyba ( za funkciou za viacnasobnym priradenim")
                                return SYN_ERROR;
                            } 

                            if ((retvalue = f_params()) != TRUE){
                                DEBUG_MODE("Zle parametre funkcie za viacnasobnym priradenim fukncie")
                                return retvalue;
                            }
                            
                        }

                        //id, ... = ..., expr
                        else {

                            
                            while (id_counter != 0){
                               if ((retvalue = expr()) != TRUE){
                                    DEBUG_MODE("Expr za viacnasobnym priradenim nespravna")
                                    return retvalue;
                    }

                                id_counter--;
                                
                                //bude treba dalsiu <expr>
                                if (id_counter != 0){

                                    token = get_token(GET, &EOL_flag, &atr);
                                    if (token == LEX_ERROR){
                                        return LEX_ERROR;
                                    }
                                    if (token != COMA || EOL_flag != NO_EOL) {
                                        DEBUG_MODE("Chyba , v rade expr, popr. zly pocet expr")
                                        return SYN_ERROR;
                                        }  
                                    }
                            }
                            DEBUG_MODE("Pocet id a <expr> sedi")
                        }
                        
                        
                        break;
                    
                

                default:
                    DEBUG_MODE("Neocakavany statement po id")
                    return SYN_ERROR;
            }
            break;

        //wall of code
            
        //if...
        case IF:
            
            if(EOL_flag != EOL_FOUND){
                return SYN_ERROR;
                }
            if ((retvalue = s_if()) != TRUE){
                DEBUG_MODE("Chyba v if")
                return retvalue;
            }
            break;
            
        
        //return...
        case RETURN:
            if(EOL_flag != EOL_FOUND){
                return SYN_ERROR;
            }
            if ((retvalue = s_return()) != TRUE){
                DEBUG_MODE("Chyba v return")
                return retvalue;
            }
            break;

        case FOR:
            if(EOL_flag != EOL_FOUND){
                return SYN_ERROR;
            }
            if ((retvalue = s_for()) != TRUE){
                DEBUG_MODE("Chyba vo for")
                return retvalue;
            }

        case R_BRACKET1:
            break;
        
        default:
            DEBUG_MODE("Nacitany neznamy zaciatok statementu")
            return SYN_ERROR;
                    

    }
    }
    //nacitana }, ktora ukoncuje kazdu postupnost statementov 
      
    if (token == R_BRACKET1){
    }
  /*  if ((token = get_token(GET, &EOL_flag, &atr)) == COMA){ //PLACEHOLDER
        return TRUE;
    }*/

    return TRUE;
}

int s_for(){
    
    DEBUG_MODE("STATEMENT FOR")

    //for - pozrie sa, co je za tym
    int token = get_token(PEEK, &EOL_flag, &atr);
    if (token == LEX_ERROR){
        return LEX_ERROR;
    }
    
    //vyraz nebude preskoceny
    if (token != UNDERSCORE){
        if((retvalue = f_definition()) != TRUE){
            DEBUG_MODE("Zla definicia vyrazu vo for")
            return retvalue;
        }
    }

    //for <def>;
    token = get_token(GET, &EOL_flag, &atr);
    if (token == LEX_ERROR){
        return LEX_ERROR;
    }
    
    if (token != UNDERSCORE || EOL_flag != NO_EOL){
        DEBUG_MODE("Chyba bodkociarka za definition vo for")
        return SYN_ERROR;

    }
    
    //for <def>; <expr> 
    if ((retvalue = expr()) != TRUE){
        DEBUG_MODE("Chyba v expr")
        return retvalue;
    }

    token = get_token(GET, &EOL_flag, &atr);
    if (token == LEX_ERROR){
        return LEX_ERROR;
    }

    if (token != UNDERSCORE || EOL_flag != NO_EOL){
        DEBUG_MODE("Chyba bodkociarka za expresion vo for")
        return SYN_ERROR;

    }

    token = get_token(PEEK, &EOL_flag, &atr);
    if (token == LEX_ERROR){
        return LEX_ERROR;
    }
    
    //vyraz nebude preskoceny
    if (token != L_BRACKET1){
        if((retvalue = f_assign()) != TRUE){
            DEBUG_MODE("Zle priradenie vo for")
            return retvalue;
        }
    }


    token = get_token(GET, &EOL_flag, &atr);
    if (token == LEX_ERROR){
        return LEX_ERROR;
    }

    if (token != L_BRACKET1 || EOL_flag != NO_EOL){
        DEBUG_MODE("Chyba { vo for")
        return SYN_ERROR;

    }

    if ((retvalue = s_body()) != TRUE){
        DEBUG_MODE("Zla sekvencia prikazov v tele foru")
        return retvalue;
        }

    DEBUG_MODE("For presiel")
    return TRUE;

}

int f_assign(){
    int token;
    int id_counter;
    token = get_token(GET, &EOL_flag, &atr);
    if (token == LEX_ERROR){
        return LEX_ERROR;
    }

    if (((token != IDENTIFIER) && (token != UNDERLINE)) || EOL_flag != NO_EOL){
        return SYN_ERROR;
    }  
    
    token = get_token(PEEK, &EOL_flag, &atr); //PEEK
    if (token == LEX_ERROR){
        return LEX_ERROR;
    }
    
    //id assign - jednoduché priradenie
    if (token == ASSIGN){
        token = get_token(GET, &EOL_flag, &atr); //realne nacitanie 
        if ((retvalue = expr()) != TRUE){
            DEBUG_MODE("Chyba v expr")
            return retvalue;
        }
    } 

    //viacnasovne nacitanie
    else { 
        id_counter = 1;


        token = get_token(GET, &EOL_flag, &atr);
        if (token == LEX_ERROR){
            return LEX_ERROR;
        }

        if (token != COMA || EOL_flag != NO_EOL){
            DEBUG_MODE("Zly znak za id vo viacnasobnom priradeni vo fore")
            return SYN_ERROR;
        }  

        while (1){
            token = get_token(GET, &EOL_flag, &atr);
            if (token == LEX_ERROR){
                return LEX_ERROR;
            }

            if (((token != IDENTIFIER) && (token != UNDERLINE)) || EOL_flag != NO_EOL){
                return SYN_ERROR;
            }
            
            id_counter++;
            
            token = get_token(GET, &EOL_flag, &atr);
            if (token == LEX_ERROR){
                return LEX_ERROR;
            }
            
            if (token == ASSIGN){
                break;
            }
            else if (token != COMA){
                DEBUG_MODE("Zle id alebo ciarka vo viacnasobnom priradeni vo for")
                return SYN_ERROR;
            }

        }

        //id, id, id... =
        while (id_counter != 0){
            if ((retvalue = expr()) != TRUE){
            DEBUG_MODE("Expr za viacnasobnym priradenim nespravna")
            return retvalue;
    }

            id_counter--;
                                
            //bude treba dalsiu <expr>
            if (id_counter != 0){
                token = get_token(GET, &EOL_flag, &atr);
                if (token == LEX_ERROR){
                    return LEX_ERROR;
                }
                if (token != COMA || EOL_flag != NO_EOL) {
                    DEBUG_MODE("Chyba , v rade expr, popr. zly pocet expr")
                    return SYN_ERROR;
                }  
            }
        }

    } 

    return TRUE;

}

//definicia pre for
int f_definition(){
    int token;

    //id
    token = get_token(GET, &EOL_flag, &atr);
    if (token == LEX_ERROR){
        return LEX_ERROR;
    }

    if ( token != IDENTIFIER || EOL_flag != NO_EOL){
        return SYN_ERROR;
        }

    //id :=
    token = get_token(GET, &EOL_flag, &atr);
    if (token == LEX_ERROR){
        return LEX_ERROR;
    }

    if ( token != DEFINITION || EOL_flag != NO_EOL){
        return SYN_ERROR;
        }
    
    //id := <expr>            
    if ((retvalue = expr()) != TRUE){
        DEBUG_MODE("Chyba v expr")
        return retvalue;
    }

    return TRUE;
}




// id(...
int f_params(){

    int token= get_token(GET, &EOL_flag, &atr);
    if (token == LEX_ERROR){
        return LEX_ERROR;
    }
    
    //id ()
    if (token == R_BRACKET) {
        return TRUE;
    }

    //TODO - nielen int, ale vsetky typy
    //id( term...
    
    if ((is_term(token) == TRUE)){
    
        while(1){ 
            token = get_token(GET, &EOL_flag, &atr);
            if (token == LEX_ERROR){
                return LEX_ERROR;
            }
            
            //id (terms)
            if ((token == R_BRACKET) && (EOL_flag == NO_EOL)){
                return TRUE;
            }

            //id(term, 
            else if(token == COMA){
                token =  get_token(GET, &EOL_flag, &atr);
                if (token == LEX_ERROR){
                    return LEX_ERROR;
                }


                if ( (!is_term(token)) || (EOL_flag != NO_EOL) ){
                    return SYN_ERROR;
                }
            }

            //id(term ine
            else {
                return SYN_ERROR;
            }
        }

        token = get_token(GET, &EOL_flag, &atr);

        //id(term)
        if (token == R_BRACKET){
            return TRUE;
        }


    }    
    //cokolvek ine okrem ) a termu
    else {
        return SYN_ERROR;
    }

    
    
    

}

int s_return(){
    int token;

    DEBUG_MODE("STATEMENT: RETURN")

    //return <expr>
    if ((retvalue = expr()) != TRUE){
        DEBUG_MODE("Chyba v expr")
        return retvalue;
    }

    token = get_token(PEEK, &EOL_flag, &atr);
    if (token == LEX_ERROR){
        return LEX_ERROR;
    }

    //return <expr>, 
    if (token == COMA && EOL_flag == NO_EOL){
        token = get_token(GET, &EOL_flag, &atr); //realne nacitanie
        if ((retvalue = s_return()) != TRUE){
                DEBUG_MODE("Chyba v return")
                return retvalue;
            }
    }
    
    return TRUE;
    
}

//if
int s_if(){
    int token;

    DEBUG_MODE("Statement if")

    //if <expr>
    if ((retvalue = expr()) != TRUE){
        DEBUG_MODE("Chyba v expr")
        return retvalue;
    }

    token = get_token(GET, &EOL_flag, &atr);
    
    if (token == LEX_ERROR){
        return LEX_ERROR;
    }

    //if <expr> {
    if (token != L_BRACKET1 || EOL_flag != NO_EOL){
        DEBUG_MODE("Chyba { v if")
        return SYN_ERROR;
    }

    /*if <expr>{

    }
    */

    if ((retvalue = s_body()) != TRUE){
        DEBUG_MODE("Zla sekvencia prikazov v if")
        return retvalue;
    }

    token = get_token(GET, &EOL_flag, &atr);
    if (token == LEX_ERROR){
        return LEX_ERROR;
    }

    if ((token != ELSE) || (EOL_flag != NO_EOL)){
        DEBUG_MODE("Chyba else v if")
        return FALSE;
    }

    token = get_token(GET, &EOL_flag, &atr);
    if (token == LEX_ERROR){
        return LEX_ERROR;
    }
    if (token != L_BRACKET1){
        DEBUG_MODE("Chyba { za else")
        return SYN_ERROR;
    }
       /*if <expr>{

    } else {

    }
    */
    if ((retvalue = s_body()) != TRUE){
            DEBUG_MODE("Zla sekvencia prikazov v else")
            return retvalue;
        }

    return TRUE;
}

//zacina az po precitani func ID (args) (type...
int return_params(){
    int token = get_token(GET, &EOL_flag, &atr);
    if (token == LEX_ERROR){
        return LEX_ERROR;
    } 

    if ((!is_type(token)) || EOL_flag != NO_EOL){
        return SYN_ERROR;
    }


    token = get_token(GET, &EOL_flag, &atr);
    if (token == LEX_ERROR){
        return LEX_ERROR;
    } 

    //func id (args) (type)
    if (token == R_BRACKET && EOL_flag == NO_EOL){

        //func ID (args) (return params)
        DEBUG_MODE("Koniec return parametrov najdeny")
        return TRUE;
    }

    //func ID (args) (type,
    else if (token != COMA || EOL_flag != NO_EOL){
        return SYN_ERROR;
    }
        
            
    //func ID (args) (type, type
    DEBUG_MODE("Nacitany dalsi return parameter")
    if ((retvalue = return_params()) != TRUE){
        DEBUG_MODE("Zly dalsi return parameter")
        return retvalue;
        }

    //prvy aj rekurzivne return_params ok 
    return TRUE;

}


//zacina func ID (type...
int args(){

    int token = get_token(GET, &EOL_flag, &atr);
    if (token == LEX_ERROR){
        return LEX_ERROR;
    }

    if (token != IDENTIFIER || EOL_flag != NO_EOL){
        DEBUG_MODE("Chyba v identifikatore argumentov funkcie")
        return SYN_ERROR;
    }

    //func id (id type
    token = get_token(GET, &EOL_flag, &atr);
    if (token == LEX_ERROR){
        return LEX_ERROR;
    }
    
    if((is_type(token) == FALSE) || EOL_flag != NO_EOL){
        DEBUG_MODE("Za id v argumentoch funkcie nie je typ");
        return SYN_ERROR;
    }

    token = get_token(GET, &EOL_flag, &atr);

    //func id(id type, 
    if ((token == COMA) && (EOL_flag == NO_EOL)){
            if ((retvalue = args()) != TRUE){
                DEBUG_MODE("Nespravny dalsi argument")
                return retvalue;
            } 

            DEBUG_MODE("Precitany dalsi argument funkcie")   
    }

    //func id (id type)
    else if (token == R_BRACKET && EOL_flag == NO_EOL){
        DEBUG_MODE("Najdeny koniec argumentov")
        return TRUE;
    }
    else {
        if (token == LEX_ERROR){
            return LEX_ERROR;
        }
        DEBUG_MODE("Neocakavany znak v argumentoch funkcie")
        return SYN_ERROR;
    }
    return TRUE;
}


int prolog(){
    int token = get_token(GET, &EOL_flag, &atr);
    
    if (token != PACKAGE){
        return SYN_ERROR; 
    }

    //package main
    token = get_token(GET, &EOL_flag, &atr);
    
    if (token == LEX_ERROR){
        return LEX_ERROR;
    } 

    //package eolf-flag main
    if (token == IDENTIFIER && EOL_flag == NO_EOL){
        if ((strcmp(atr.str, "main") == 0)) {
            return TRUE;
        }
        DEBUG_MODE("Package nie je main");
       
    }
    return SYN_ERROR;
}

int expr(){
    //PLACEHOLDER
    int token = get_token(GET, &EOL_flag, &atr);
    if (token == LEX_ERROR){
        return LEX_ERROR;
    }

    if ((!is_term(token))){
        return SYN_ERROR;
    }

    return TRUE;
}



