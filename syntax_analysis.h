/****
 * IFJ Projekt 2020
 * 
 * Autori:
 * xjorio00 - Vanessa Jóriová 
 * 
 * Súhrn: ústredný modul celého programu, vykonávajúci syntaktickú a sémantickú analýzu, 
 * vo svojej činnosti využívajú ostatné moduly (scaner a precedenčnú analýzy výrazov) 
 * 
 * 
 * */
#ifndef SYNRAX_ANALYSIS_H
#define SYNTAX__ANALYSIS_H

#include <stdbool.h>

void built_in_insert();
int analysis_start();
int prolog();
int func();
int args(string func_name, int mode);
int return_params(string func_name, int mode);
int s_body(int func_body, string func_name, int for_label);
int expr(int * expr_type);
int f_params(string name, int mode);
int simple_func_or_expr(int for_assign);
int func_or_expr(int for_assign);
int s_if(string funcname);
int s_return(string funcname);
int s_for(string funcname);
int f_definition();
int f_assign();
int program();
int s_body_r(int func_body, string func_name, int for_label);
int s_definition(int func_body, int for_label);



#endif