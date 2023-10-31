%{
#include "tokens.h"
#include "hash.h"

/*
* Etapa 1 - scanner.l
* INF-UFRGS - INF01147 Compiladores - 2023/2
* Guilherme Klein Kern
*/

int m_is_running = 1;
int m_curr_line = 1;
%}

escape \\[bnrt'\"\\]
number 0|([1-9][0-9]*)
digit  [0-9]
whitespace [ \t]

%x SINGLE_COMMENT
%x MULTI_COMMENT

%%

char       {return KW_CHAR;}
int        {return KW_INT;}
float      {return KW_FLOAT;}

code       {return KW_CODE;}

if         {return KW_IF;}
else       {return KW_ELSE;}
while      {return KW_WHILE;}
input      {return KW_INPUT;}
print      {return KW_PRINT;}
return     {return KW_RETURN;}

[-,;()\[\]{}=+*/%<>&|~]  {return yytext[0];}

">="         {return OPERATOR_GE;}
"<="         {return OPERATOR_LE;}
"=="         {return OPERATOR_EQ;}
"!="         {return OPERATOR_DIF;}

{number}	{
				add_symbol(LIT_INT, yytext);
				return LIT_INT;
			}

[0-9a-zA-Z_]+   {
					add_symbol(TK_IDENTIFIER, yytext);
					return TK_IDENTIFIER;
				}

{number}\.{digit}+ 	{
						add_symbol(LIT_FLOAT, yytext);
						return LIT_FLOAT;
					}

'({escape}|[^\"'\\])'   {
							add_symbol(LIT_CHAR, yytext);
							return LIT_CHAR;
						}
		
\"({escape}|[^"])*\"    {
							add_symbol(LIT_STRING, yytext);
							return LIT_STRING;
						}

\n                  {m_curr_line++;}
{whitespace}
"//"			    {BEGIN(SINGLE_COMMENT);}
"///"               {BEGIN(MULTI_COMMENT);}
.                   {return TOKEN_ERROR;}

<SINGLE_COMMENT>\n         {m_curr_line++;BEGIN(INITIAL);}
<SINGLE_COMMENT>.

<MULTI_COMMENT>\\\\\\      {BEGIN(INITIAL);}
<MULTI_COMMENT>\n          {m_curr_line++;}
<MULTI_COMMENT>.
%%

void initMe(void) {
	init_symbol_table();
}

int getLineNumber(void) {
	return m_curr_line;
}

int isRunning(void) {
	return m_is_running;
}

int yywrap() {
    m_is_running = 0;
    return 1;
}



