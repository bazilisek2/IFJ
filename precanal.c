/****
 * IFJ Projekt 2020
 * 
 * Autori:
 * xbella01 - Magdaléna Bellayová 
 * 
 * Súhrn: Precedenčná analýza
 * Prechádza tokeny vo výraze a uplatňuje na nich pravidlá  
 * 
 * */

#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include <stdbool.h>
#include <string.h>
#include "scanner.h"
#include "global.h"

#include "codegen.h"
#include "precanal.h"
#include "stack.h"
#include "str.h"
#include "symtable.h"
#include "error.h"

#define RULE_COUNTER 14 //pocet termov v tabulke
#define BOOL 0

//enumeracia stavov z precedencnej tabulky -rovna sa, vacsi, mensi, error
enum {
	EQ,
	LO,
	HI,
	ER
};

//precedencna tabulka        
const char prece_table[RULE_COUNTER][RULE_COUNTER] = {
/*         +   -   *   /   (   )   id  <   >   <=  >=  ==  !=  $*/
/* +  */ { HI, HI, LO, LO, LO, HI, LO, HI, HI, HI, HI, HI, HI, HI },
/* -  */ { HI, HI, LO, LO, LO, HI, LO, HI, HI, HI, HI, HI, HI, HI },
/* *  */ { HI, HI, HI, HI, LO, HI, LO, HI, HI, HI, HI, HI, HI, HI },
/* /  */ { HI, HI, HI, HI, LO, HI, LO, HI, HI, HI, HI, HI, HI, HI },
/* (  */ { LO, LO, LO, LO, LO, EQ, LO, LO, LO, LO, LO, LO, LO, ER },
/* )  */ { HI, HI, HI, HI, ER, HI, ER, HI, HI, HI, HI, HI, HI, HI },
/* id */ { HI, HI, HI, HI, ER, HI, ER, HI, HI, HI, HI, HI, HI, HI },
/* <  */ { LO, LO, LO, LO, LO, HI, LO, HI, HI, HI, HI, HI, HI, HI },
/* >  */ { LO, LO, LO, LO, LO, HI, LO, HI, HI, HI, HI, HI, HI, HI },
/* <= */ { LO, LO, LO, LO, LO, HI, LO, HI, HI, HI, HI, HI, HI, HI },
/* >= */ { LO, LO, LO, LO, LO, HI, LO, HI, HI, HI, HI, HI, HI, HI },
/* == */ { LO, LO, LO, LO, LO, HI, LO, LO, LO, LO, LO, HI, HI, HI },
/* != */ { LO, LO, LO, LO, LO, HI, LO, LO, LO, LO, LO, HI, HI, HI },
/* $  */ { LO, LO, LO, LO, LO, ER, LO, LO, LO, LO, LO, LO, LO, ER },
};

int result_type; //vysledny typ
string atr; //atribut v ktorom sa uklada hodnota
int EOL_flag; //flag konca riadka


//hladanie v precedencnej tabulke, pomocou vrchnej hodnoty na stacku (stlpec) a tokenu zo vstupu (riadok)
int prectable_search(tStack *S,int token)
{

	int r;
	int c;
	tStack_element* e1 = topterm_stack(S);
	switch(e1->symbol)
	{
		case IDENTIFIER:
		case INT:
		case FLOAT_N:
		case FLOAT_L:
		case STRING_LIT:
		case IDENT:
			r = 6;
			break;
		case L_BRACKET:
			r = 4;
			break;
		case R_BRACKET:
			r = 5;
			break;
		case PLUS:
			r = 0;
			break;
		case MINUS:
			r = 1;
			break;
		case MULTIPLICATION:
			r = 2;
			break;
		case DIVISION:
			r = 3;
			break;
		case EQUAL:
			r = 11;
			break;
		case NOTEQUAL:
			r = 12;
			break;
		case LESSEQUAL:
			r = 9;
			break;
		case GREATEQUAL:
			r = 10;
			break;
		case GREAT:
			r = 8;
			break;
		case LESS:
			r = 7;
			break;
		case DOLLAR:
			r = 13;
			break;
		default:
			return SYN_ERROR;

	}

	switch(token)
	{
		case IDENTIFIER:
		case INT:
		case FLOAT_N:
		case FLOAT_L:
		case STRING_LIT:
		case IDENT:
			c = 6;
			break;
		case L_BRACKET:
			c = 4;
			break;
		case R_BRACKET:
			c = 5;
			break;
		case PLUS:
			c = 0;
			break;
		case MINUS:
			c = 1;
			break;
		case MULTIPLICATION:
			c = 2;
			break;
		case DIVISION:
			c = 3;
			break;
		case EQUAL:
			c = 11;
			break;
		case NOTEQUAL:
			c = 12;
			break;
		case LESSEQUAL:
			c = 9;
			break;
		case GREATEQUAL:
			c = 10;
			break;
		case GREAT:
			c = 8;
			break;
		case LESS:
			c = 7;
			break;
		case DOLLAR:
			c = 13;
			break;
		default:
			return SYN_ERROR;

	}
	return prece_table[r][c];
}

//uplatnovanie pravidiel
tStack_element* rules_apl(tStack* S, int count)
{
	tStack_element* result = NULL;

	if(count==1)
	{
		tStack_element* op1 = top_stack(S);
		if (op1->symbol == IDENTIFIER)
		{
			switch(op1->type)
			{
				case INT:
					codegen_push (op1->symbol, op1->value.str);
                			push_stack(S, NONTERMINAL, INT, &(op1->value));
               				result = top_stack(S);
                			return result;
					break;
				case FLOAT_N:
				case FLOAT_L:
					codegen_push (op1->symbol, op1->value.str);
                			push_stack(S, NONTERMINAL, FLOAT_N,&(op1->value));
                			result = top_stack(S);
					return result;
					break;
				case STRING_LIT:
					codegen_push (op1->symbol, op1->value.str);
                			push_stack(S, NONTERMINAL, STRING_LIT,&(op1->value));
               				result = top_stack(S);
					return result;
					break;
			}
		}
		if (op1->symbol == IDENT)
		{
			switch(op1->type)
			{
				case INT:
					codegen_push (op1->type, op1->value.str);
               	 			push_stack(S, NONTERMINAL, INT, &(op1->value));
               				result = top_stack(S);
                			return result;
					break;
				case FLOAT_N:
				case FLOAT_L:
					codegen_push (op1->type, op1->value.str);
        	        		push_stack(S, NONTERMINAL, FLOAT_N,&(op1->value));
        	        		result = top_stack(S);
					return result;
					break;
				case STRING_LIT:
					codegen_push (op1->type, op1->value.str);
                			push_stack(S, NONTERMINAL, STRING_LIT,&(op1->value));
               				result = top_stack(S);
					return result;
					break;
			}
		}

	}
	else if(count==3)
	{

		tStack_element* op1 = S->top;
		tStack_element* op2 = S->top->next;
		tStack_element* op3 = S->top->next->next;

		if(op1->symbol == L_BRACKET && op2->symbol == NONTERMINAL && op3->symbol == R_BRACKET )
		{
                	push_stack(S, op2->symbol, op2->type, &(op2->value));
                	result = top_stack(S);
			return result;
		}
		if(op1->symbol==NONTERMINAL && op3->symbol==NONTERMINAL)
		{
			if(op1->type == op3->type)
			{
				switch(op2->symbol)
				{
					case PLUS:

						switch(op1->type)
						{
							case INT:
								push_stack(S,NONTERMINAL, INT,&(op1->value));
                       		    				result = top_stack(S);
								codegen_operation (op2->symbol);
								return result;
								break;
							case FLOAT_N:
							case FLOAT_L:
								push_stack(S,NONTERMINAL, FLOAT_N,&(op1->value));
                            					result = top_stack(S);
								codegen_operation (op2->symbol);
								return result;
								break;
							case STRING_LIT:
								push_stack(S,NONTERMINAL, STRING_LIT,&(op1->value));
                        					result = top_stack(S);
								codegen_operation (CONCAT);
								return result;
								break;
							default:
                      					        push_stack(S,TYPE_ERROR, 0, &(op1->value));
                                				result = top_stack(S);
                                				return result;

						}
                    				break;

					case MINUS:
						codegen_operation (op2->symbol);
						switch(op1->type)
						{
							case INT:
								push_stack(S,NONTERMINAL, INT,&(op1->value));
                                				result = top_stack(S);
								return result;
								break;
							case FLOAT_N:
							case FLOAT_L:
								push_stack(S,NONTERMINAL, FLOAT_N,&(op1->value));
                                				result = top_stack(S);
								return result;
								break;
							default:
                      				        	push_stack(S,TYPE_ERROR, 0, &(op1->value));
                                				result = top_stack(S);
								return result;
						}
                        			break;
					case MULTIPLICATION:
						codegen_operation (op2->symbol);
						switch(op1->type)
						{
							case INT:
								push_stack(S,NONTERMINAL, INT,&(op1->value));
                                				result = top_stack(S);
							    	return result;
								break;
							case FLOAT_N:
							case FLOAT_L:
								push_stack(S,NONTERMINAL, FLOAT_N,&(op1->value));
                                				result = top_stack(S);
								return result;
								break;
							default:
								push_stack(S,TYPE_ERROR, 0, &(op1->value));
                                				result = top_stack(S);
                                				return result;

						}
                    				break;

					case DIVISION:
						//ERR_ZERODIV
						switch(op1->type)
						{
							case INT:
								push_stack(S,NONTERMINAL, INT,&(op1->value));
                            					result = top_stack(S);
								codegen_operation (IDIV);
								return result;
								break;
							case FLOAT_N:
							case FLOAT_L:
								push_stack(S,NONTERMINAL, FLOAT_N,&(op1->value));
                            					result = top_stack(S);
								codegen_operation (DIV);
								return result;
								break;
							default:
                      					        push_stack(S,TYPE_ERROR, 0, &(op1->value));
                                				result = top_stack(S);
                                				return result;
						}
                    				break;

					case EQUAL:
					case NOTEQUAL:
					case LESSEQUAL:
					case GREATEQUAL: 
					case LESS:
					case GREAT:
						codegen_operation (op2->symbol);
						push_stack(S,NONTERMINAL, BOOL,&(op1->value));
                            			result = top_stack(S);
						return result;
						break;

				}
			}
                        else
                        {
                        	push_stack(S,TYPE_ERROR, 0, &(op1->value));
                                result = top_stack(S);
                                return result;

                        }
		}
	}
	return result;	
}

//hladanie hodnot v precedenčnej tabulke a podla stavu z tabulky nasledne uplatnovanie pravidiel
int expression(int* flag,tTableList* TL)
{
	bool correct = FALSE;
	int symbol;
	strInit(&atr);
	tStack* s1 = malloc(sizeof(tStack));
	tStack* s2 = malloc(sizeof(tStack));
	init_stack(s1);
	do 
	{

		int token = get_token(PEEK, &EOL_flag, &atr);
		int datatype = 0;
		if (token == LEX_ERROR)
		{
			return LEX_ERROR;
		}
		if((token == COMA) || EOL_flag != NO_EOL || token == UNDERSCORE || token == L_BRACKET1)
		{
			token = DOLLAR;
		}
		if(token==IDENTIFIER)
		{
			tNode* variable = tablelist_search(TL, &atr);
			if(variable == NULL)
			{
				return SEM_ERROR;
			}

			if(variable->data.function == TRUE)
			{
                		return SEM_ERROR;
            		}
			strCopyString(&atr, &(variable->code_gen_key));
			datatype = variable->data.type;
		}
        	else if (token==INT || token == FLOAT_N || token == FLOAT_L || token== STRING_LIT)
		{
			datatype = token;
			token = IDENT;	
		}

		tStack_element* top_term = topterm_stack(s1);
		do
		{
			symbol = prectable_search(s1,token);
                	if(symbol==SYN_ERROR)
                	{		
                        	return SYN_ERROR;

                	}
			switch(symbol)
			{
				case LO:
					if(s1->top->symbol == NONTERMINAL)
					{
						insertbeforetopterm_stack(s1,LO,datatype,&atr);
						push_stack(s1, token, datatype, &atr);
					}
					else
					{
						push_stack(s1,LO,datatype, &atr);
						push_stack(s1, token, datatype, &atr);
					}	
					break;
				case HI:
					init_stack(s2);
					int count = 0;
					while(s1->top->symbol != LO)
					{
						push_stack(s2,s1->top->symbol, s1->top->type, &(s1->top->value));
                   				pop_stack(s1);
						count++;
					}
					pop_stack(s1);
					tStack_element* result = rules_apl(s2, count);
					if(result == NULL)
					{
						return SYN_ERROR;
			                }
                			if(result->symbol == TYPE_ERROR)
					{
                        			return TYPE_ERROR;
                			}
					push_stack(s1,result->symbol,result->type,&(result->value));
					break;
				case EQ:
					push_stack(s1, token, datatype, &atr);
					break;
				case ER:
					return SYN_ERROR;
					break; 
			}
			top_term = topterm_stack(s1);

		}while(symbol == HI && !(top_term->symbol==token && token == DOLLAR));	
		
		if(s1->top->symbol != token)
		{
                	push_stack(s1, token, datatype, &atr);	
		}
		if(token == DOLLAR)
		{
			pop_stack(s1);
			tStack_element* e1 = topterm_stack(s1);
			if(e1->symbol == DOLLAR)
			{
				tStack_element* e2 = top_stack(s1);
				if(e2->symbol == NONTERMINAL)
				{
					correct = TRUE;
					result_type = e2->type;
				}
			}
		}
		else
		{
			token = get_token(GET, &EOL_flag, &atr);
		}
	}while(!correct);
	free_stack(s1);
	free_stack(s2);
	*flag = result_type;
	DEBUG_MODE("Uspesne ukoncena analyza")
	return TRUE;
} 
