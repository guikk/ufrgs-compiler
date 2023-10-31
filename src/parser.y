%{
	#include <stdlib.h>
	#include "main.h"
/*
* Etapa 2 - parser.y
* INF-UFRGS - INF01147 Compiladores - 2023/2
* Guilherme Klein Kern
*/
%}

%token KW_CHAR
%token KW_INT
%token KW_FLOAT

%token KW_CODE

%token KW_IF
%token KW_ELSE
%token KW_WHILE
%token KW_INPUT
%token KW_PRINT
%token KW_RETURN

%token OPERATOR_LE
%token OPERATOR_GE
%token OPERATOR_EQ
%token OPERATOR_DIF

%token TK_IDENTIFIER

%token LIT_INT
%token LIT_FLOAT
%token LIT_CHAR
%token LIT_STRING

%token TOKEN_ERROR

%%

program : decl_list func_impl_list
		;

type : KW_CHAR
	 | KW_FLOAT
	 | KW_INT
	 ;

typed_id : type TK_IDENTIFIER
		 ;

value : LIT_CHAR
		   | LIT_FLOAT
		   | LIT_INT
		   ;

decl_list : decl decl_list
		  |
		  ;

decl : var_decl
	 | vec_decl
	 | func_decl
	 ;

var_decl : typed_id '=' value ';'
		 ;

vec_decl : typed_id '[' LIT_INT ']' vec_init ';'
		 ;

vec_init : value vec_init
		 |
		 ;

func_decl : typed_id '(' ')' ';'
		  | typed_id '(' param_list ')' ';'
		  ;

param_list : typed_id ',' param_list
		   | typed_id
		   ;

func_impl_list : func_impl func_impl_list
			   |
			   ;

func_impl : KW_CODE TK_IDENTIFIER scope
		  ;

scope : '{' statemet_list '}'

statemet_list : statement statemet_list
			  |
			  ;

statement : assignment
		  | if_st
		  | if_else_st
		  | while_st
		  ;

assignment : TK_IDENTIFIER '=' expr ';'
		   | TK_IDENTIFIER '[' expr ']' '=' expr ';'
		   ;

expr : value
	 ;



if_st : KW_IF '(' expr ')' scope
	  ;

if_else_st : if_then KW_ELSE scope
	  	   ;

while_st : KW_WHILE '(' expr ')' scope
	     ;

%%

int yyerror(char *err){
	fprintf(stderr, "%s:%d: %s\n", get_input_filename(), getLineNumber(), err);
	exit(ERR_BAD_SYNTAX);
}