#ifndef CODE_GEN_H
#define CODE_GEN_H


//funkcie pouzivane ku generovaniu kodu
#define BOOL 0
void codegen_start();
void codegen_end();
void codegen_write(int type, char* value);
void codegen_push(int type, char* value);
void codegen_declare(char * id);
void codegen_assign(char * id);
void codegen_operation(int operation);
void codegen_main();
void codegen_call(char* func);
void codegen_inputs();
void codegen_inputf();
void codegen_inputi();
void codegen_call_inputs();
void codegen_funcreturn(char * return_id, int order);
void codegen_createframe();
void codegen_param(int type, char * value, int order);
void codegen_len();
void codegen_int2float();
void codegen_float2int();
void codegen_chr();
void codegen_ord();
void codegen_substr();
void codegen_if_start(int label);
void codegen_else_skip(int label);
void codegen_else_label(int label);
void codegen_if_end(int label);
void codegen_funclabel(char * label);
void codedegen_for_expr(int label);
void codegen_for_assign_start(int label);
void codegen_for_assign_end(int label);
void codegen_for_cycle(int label);
void codegen_for_check(int label);
void codegen_for_end(int label);
void codegen_for_start(int label);
void codegen_func_param(int order, char * name);
void codegen_func_return(int order);
void codegen_func_end();
void codegen_for_defvar(int label, char * id);

#endif