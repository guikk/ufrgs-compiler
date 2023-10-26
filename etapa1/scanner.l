%{
	#include "tokens.h"
	#include "hash.h"

	int m_is_running = 1;
	int m_curr_line = 1;
%}

%x COMMENT

%%

byte						{return KW_BYTE;      }
int                         {return KW_INT;       }
float                       {return KW_FLOAT;     }
if                          {return KW_IF;        }
then                        {return KW_THEN;      }
else                        {return KW_ELSE;      }
loop                        {return KW_LOOP;      }
leap                        {return KW_LEAP;      }
read                        {return KW_READ;      }
print                       {return KW_PRINT;     }
return                      {return KW_RETURN;    }
[-,;:()\[\]{}+*/<>=&$#]     {return yytext[0];    }
or                          {return OPERATOR_OR;  }
and                         {return OPERATOR_AND; }
not                         {return OPERATOR_NOT; }
">="                        {return OPERATOR_GE;  }
"<="                        {return OPERATOR_LE;  }
"=="                        {return OPERATOR_EQ;  }
"!="                        {return OPERATOR_DIF; }
[0-9a-z_]+                  {hashInsert(TK_IDENTIFIER, yytext); return TK_IDENTIFIER;}
[A-IO]+						{hashInsert(LIT_INTEGER, yytext); return LIT_INTEGER;  }
[A-IO]+\.[A-IO]+            {hashInsert(LIT_FLOAT, yytext); return LIT_FLOAT;    }
\n                          {LineNumber++;        }
[ \t]
ZZ.*
XXX                         {BEGIN(COMMENT);}
'.'                         {hashInsert(LIT_CHAR, yytext); return LIT_CHAR;     }
\"(\\.|[^"])*\"             {hashInsert(LIT_STRING, yytext); return LIT_STRING;   }
.                           {return TOKEN_ERROR;  }

<COMMENT>XXX                {BEGIN(INITIAL);}
<COMMENT>.
<COMMENT>\n                 {m_curr_line++;}

%%

void initMe(void){
	hashInit();
}

int getLineNumber(void){
	return m_curr_line;
}

int isRunning(void){
	return m_is_running;
}

int yywrap(){
    m_is_running = 0;
    return 1;
}



