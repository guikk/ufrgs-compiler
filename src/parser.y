%{
	#include <stdlib.h>
	#include "main.h"
	int yyerror();
	int yylex();
	extern int getLineNumber();
/*
 * Etapa 3 - parser.y
 * INF-UFRGS - INF01147 Compiladores - 2023/2
 * Guilherme Klein Kern
 */

	ast_node* root;
%}

%union{
	symbol_node*s ymbol;
	ast* tree;
}

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

%token<symbol> TK_IDENTIFIER

%token<symbol> LIT_INT
%token<symbol> LIT_FLOAT
%token<symbol> LIT_CHAR
%token<symbol> LIT_STRING

%token TOKEN_ERROR

%left '*' '/'
%left '+' '-'
%left OPERATOR_GE OPERATOR_LE OPERATOR_EQ OPERATOR_DIF
%left '<' '>' '&' '|' '~'

%type<tree> program
%type<tree> type

%%

program : decl_list func_impl_list    {$$=ast_new(AST_PROGRAM,0,$1,$2,0,0)}
		;

type : KW_CHAR    {$$=ast_new(AST_CHAR_T,0,0,0,0,0)}
	 | KW_FLOAT   {$$=ast_new(AST_FLOAT_T,0,0,0,0,0)}
	 | KW_INT     {$$=ast_new(AST_INT_T,0,0,0,0,0)}
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

var_decl : type TK_IDENTIFIER '=' value ';'
		 ;

vec_decl : type TK_IDENTIFIER '[' LIT_INT ']' vec_init ';'
		 ;

vec_init : vec_init_values
		 |
		 ;

vec_init_values : value vec_init_values
				| value 
				;

func_decl : type TK_IDENTIFIER '(' ')' ';'
		  | type TK_IDENTIFIER '(' param_list ')' ';'
		  ;

param_list : type TK_IDENTIFIER ',' param_list
		   | type TK_IDENTIFIER
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
		| ';'
		;

assignment : TK_IDENTIFIER '=' expr ';'
		   | TK_IDENTIFIER '[' expr ']' '=' expr ';'
		   ;		

expr : TK_IDENTIFIER
	 | TK_IDENTIFIER '[' expr ']'
	 | value
	 | '(' expr ')'
	 | expr '+' expr
	 | expr '-' expr
	 | expr '*' expr
	 | expr '/' expr
	 | expr '<' expr
	 | expr '>' expr
     | expr OPERATOR_GE expr
     | expr OPERATOR_LE expr
     | expr OPERATOR_EQ expr
     | expr OPERATOR_DIF expr
	 | expr '&' expr
	 | expr '|' expr
	 | '~' expr
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