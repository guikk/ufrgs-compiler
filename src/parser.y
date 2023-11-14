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

vec_decl : typed_id '[' LIT_INT ']' ';'
		 | typed_id '[' LIT_INT ']' '=' vec_init ';'
		 ;

vec_init : value vec_init
		 | value
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

func_impl : KW_CODE TK_IDENTIFIER command
		  ;

command : assignment
		| KW_PRINT LIT_STRING ';'
		| KW_PRINT expr ';'
		| KW_RETURN expr ';'
		| scope
		;

assignment : TK_IDENTIFIER '=' expr ';'
		   | TK_IDENTIFIER '[' expr ']' '=' expr ';'
		   ;		

expr : TK_IDENTIFIER
	 | TK_IDENTIFIER '[' expr ']'
	 | value
	 | expr '+' expr
	 | expr '-' expr
	 | expr '*' expr
	 | expr '/' expr
	 | expr '<' expr
	 | expr '>' expr
	 | expr '<=' expr
	 | expr '>=' expr
	 | expr '==' expr
	 | expr '!=' expr
	 | expr '&' expr
	 | expr '|' expr
	 | expr '~' expr
	 | TK_IDENTIFIER '(' arg_list ')'
	 | KW_INPUT '(' type ')'
	 ;

arg_list : expr
		 | expr ',' arg_list 
		 ;

scope : '{' statement_list '}'  


statement_list : statement statement_list
			  |
			  ;

statement : command 
		  | if_st
		  | if_else_st
		  | while_st
		  ;

if_st : KW_IF '(' expr ')' command
	  ;

if_else_st : if_st KW_ELSE command
	  	   ;

while_st : KW_WHILE '(' expr ')' command
	     ;

%%

int yyerror(char *err){
	fprintf(stderr, "%s:%d: %s\n", get_input_filename(), getLineNumber(), err);
	exit(ERR_BAD_SYNTAX);
}