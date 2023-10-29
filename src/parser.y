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

program : declaration_list
		;

declaration_list : declaration declaration_list
		  		 |
		  		 ;

declaration : type TK_IDENTIFIER ';'
			| type TK_IDENTIFIER assignment ';'
			;

assignment : '=' literal

type : KW_CHAR
	 | KW_FLOAT
	 | KW_INT
	 ;

literal : LIT_CHAR
		| LIT_FLOAT
		| LIT_INT
		| LIT_STRING
		;

%%

int yyerror(char *err){
	fprintf(stderr, "%d: Error %s\n", getLineNumber(), err);
	exit(ERR_BAD_SYNTAX);
}