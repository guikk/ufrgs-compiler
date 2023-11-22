%{
	#include <stdlib.h>
	#include "main.h"
	#include "symbol_table.h"
	#include "ast.h"
	int yyerror();
	int yylex();
	extern int getLineNumber();
/*
 * Etapa 3 - parser.y
 * INF-UFRGS - INF01147 Compiladores - 2023/2
 * Guilherme Klein Kern
 */

	ast* root;
%}

%union{
	symbol_node* symbol;
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
%type<tree> value
%type<tree> decl_list 
%type<tree> decl
%type<tree> var_decl
%type<tree> vec_decl
%type<tree> vec_init
%type<tree> vec_init_values
%type<tree> func_decl
%type<tree> param_list
%type<tree> func_impl_list
%type<tree> func_impl
%type<tree> command
%type<tree> assignment
%type<tree> expr
%type<tree> arg_list
%type<tree> scope
%type<tree> statement_list
%type<tree> statement
%type<tree> if_st
%type<tree> if_else_st
%type<tree> while_st

%%

program : decl_list func_impl_list    {$$=ast_new(AST_PROGRAM,0,$1,$2,0,0); root=$$;}
		;

type : KW_CHAR    {$$=ast_new(AST_CHAR_T,0,0,0,0,0);}
	 | KW_FLOAT   {$$=ast_new(AST_FLOAT_T,0,0,0,0,0);}
	 | KW_INT     {$$=ast_new(AST_INT_T,0,0,0,0,0);}
	 ;

value : LIT_CHAR    {$$=ast_new(AST_SYMBOL,$1,0,0,0,0);}
	  | LIT_FLOAT   {$$=ast_new(AST_SYMBOL,$1,0,0,0,0);}
	  | LIT_INT     {$$=ast_new(AST_SYMBOL,$1,0,0,0,0);}
	  ;

decl_list : decl decl_list   {$$=ast_new(AST_DECL_L,0,$1,$2,0,0);}
		  |					 {$$=0;}
		  ;

decl : var_decl    {$$=$1;}
	 | vec_decl    {$$=$1;}
	 | func_decl   {$$=$1;}
	 ;

var_decl : type TK_IDENTIFIER '=' value ';'  {$$=ast_new(AST_DECL_VAR,$2,$1,$4,0,0);}
		 ;

vec_decl : type TK_IDENTIFIER '[' LIT_INT ']' vec_init ';' {$$=ast_new(AST_DECL_VEC,$2,$1,ast_new(AST_SYMBOL,$4,0,0,0,0),$6,0);}
		 ;

vec_init : vec_init_values    {$$=$1;}
		 |                    {$$=0;}
		 ;

vec_init_values : value vec_init_values {$$=ast_new(AST_VECINIT_L,0,$1,$2,0,0);}
				| value                 {$$=ast_new(AST_VECINIT_L,0,$1,0,0,0);}
				;

func_decl : type TK_IDENTIFIER '(' ')' ';'               {$$=ast_new(AST_DECL_FUNC,$2,$1,0,0,0);}
		  | type TK_IDENTIFIER '(' param_list ')' ';'    {$$=ast_new(AST_DECL_FUNC,$2,$1,$4,0,0);}
		  ;

param_list : type TK_IDENTIFIER ',' param_list    {$$=ast_new(AST_PARAM_L,$2,$1,$4,0,0);}
		   | type TK_IDENTIFIER                   {$$=ast_new(AST_PARAM_L,$2,$1,0,0,0);}
		   ;

func_impl_list : func_impl func_impl_list    {$$=ast_new(AST_FUNC_L,0,$1,$2,0,0);}
			   |                             {$$=0;}
			   ;

func_impl : KW_CODE TK_IDENTIFIER command    {$$=ast_new(AST_FUNC,$2,$3,0,0,0);}
		  ;

command : assignment                 {$$=$1;}
		| KW_PRINT LIT_STRING ';'    {$$=ast_new(AST_PRINT,0,ast_new(AST_SYMBOL,$2,0,0,0,0),0,0,0);}
		| KW_PRINT expr ';'          {$$=ast_new(AST_PRINT,0,$2,0,0,0);}
		| KW_RETURN expr ';'         {$$=ast_new(AST_RETURN,0,$2,0,0,0);}
		| scope                      {$$=$1;}
		| ';'                        {$$=ast_new(AST_EMPTYCMD,0,0,0,0,0);}
		;

assignment : TK_IDENTIFIER '=' expr ';'                 {$$=ast_new(AST_ASSIGN,$1,$3,0,0,0);}
		   | TK_IDENTIFIER '[' expr ']' '=' expr ';'    {$$=ast_new(AST_VECASSIGN,$1,$3,$6,0,0);}
		   ;		

expr : TK_IDENTIFIER                     {$$=ast_new(AST_SYMBOL,$1,0,0,0,0);}
	 | TK_IDENTIFIER '[' expr ']'        {$$=ast_new(AST_VECELEM,$1,$3,0,0,0);}
	 | value                             {$$=$1;}
	 | '(' expr ')'                      {$$=ast_new(AST_PAREXPR,0,$2,0,0,0);}
	 | expr '+' expr                     {$$=ast_new(AST_SUM,0,$1,$3,0,0);}
	 | expr '-' expr                     {$$=ast_new(AST_SUB,0,$1,$3,0,0);}
	 | expr '*' expr                     {$$=ast_new(AST_MUL,0,$1,$3,0,0);}
	 | expr '/' expr                     {$$=ast_new(AST_DIV,0,$1,$3,0,0);}
	 | expr '<' expr                     {$$=ast_new(AST_LT,0,$1,$3,0,0);}
	 | expr '>' expr                     {$$=ast_new(AST_GT,0,$1,$3,0,0);}
     | expr OPERATOR_GE expr             {$$=ast_new(AST_GE,0,$1,$3,0,0);}
     | expr OPERATOR_LE expr             {$$=ast_new(AST_LE,0,$1,$3,0,0);} 
     | expr OPERATOR_EQ expr             {$$=ast_new(AST_EQ,0,$1,$3,0,0);}
     | expr OPERATOR_DIF expr            {$$=ast_new(AST_DIF,0,$1,$3,0,0);}
	 | expr '&' expr                     {$$=ast_new(AST_AND,0,$1,$3,0,0);}
	 | expr '|' expr                     {$$=ast_new(AST_OR,0,$1,$3,0,0);}
	 | '~' expr                          {$$=ast_new(AST_NOT,0,$2,0,0,0);}
	 | TK_IDENTIFIER '(' arg_list ')'    {$$=ast_new(AST_FUNCCALL,$1,$3,0,0,0);}
	 | KW_INPUT '(' type ')'             {$$=ast_new(AST_INPUT,0,$3,0,0,0);}
	 ;

arg_list : expr                 {$$=ast_new(AST_ARG_L,0,$1,0,0,0);}
		 | expr ',' arg_list    {$$=ast_new(AST_ARG_L,0,$1,$3,0,0);} 
		 ;

scope : '{' statement_list '}'  {$$=ast_new(AST_SCOPE,0,$2,0,0,0);}


statement_list : statement statement_list    {$$=ast_new(AST_STMT_L,0,$1,$2,0,0);}
			   |                             {$$=0;}
			   ;

statement : command       {$$=$1;}
		  | if_st         {$$=$1;}
		  | if_else_st    {$$=$1;}
		  | while_st      {$$=$1;}
		  ;

if_st : KW_IF '(' expr ')' command    {$$=ast_new(AST_IF,0,$3,$5,0,0);}
	  ;

if_else_st : KW_IF '(' expr ')' command KW_ELSE command    {$$=ast_new(AST_IFELSE,0,$3,$5,$7,0);}
	  	   ;

while_st : KW_WHILE '(' expr ')' command    {$$=ast_new(AST_WHILE,0,$3,$5,0,0);}
	     ;

%%

int yyerror(char *err){
	fprintf(stderr, "%s:%d: %s\n", get_input_filename(), getLineNumber(), err);
	exit(ERR_BAD_SYNTAX);
}

ast* get_parsed_ast() {
    return root;
}