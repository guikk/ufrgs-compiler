%{
#include "tokens.h"
#include "hash.h"

/*
* Etapa 1 - tokens.h
* INF-UFRGS - INF01147 Compiladores - 2023/2
* Guilherme Klein Kern
*/

int m_is_running = 1;
int m_curr_line = 1;
%}

%x COMMENT

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

[A-IO]+	            {
						hashInsert(LIT_INT, yytext);
						return LIT_INT;
					}

[0-9a-z_]+          {
						hashInsert(TK_IDENTIFIER, yytext);
						return TK_IDENTIFIER;
					}

[A-IO]+\.[A-IO]+ 	{
						hashInsert(LIT_REAL, yytext);
						return LIT_REAL;
					}

'.'                 {
						hashInsert(LIT_CHAR, yytext);
						return LIT_CHAR;
					}
		
\"(\\.|[^"])*\"     {
						hashInsert(LIT_STRING, yytext);
						return LIT_STRING;
					}

\n                          {m_curr_line++;}
[ \t]
ZZ.*
XXX                         {BEGIN(COMMENT);}
.                           {return TOKEN_ERROR;  }

<COMMENT>XXX                {BEGIN(INITIAL);}
<COMMENT>.
<COMMENT>\n                 {m_curr_line++;}

%%

void initMe(void) {
	hashInit();
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



