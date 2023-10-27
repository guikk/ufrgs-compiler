/*
 * Etapa 1 - main.c
 * INF-UFRGS - INF01147 Compiladores - 2023/2
 * Guilherme Klein Kern
 */

#include <stdio.h>
#include "lex.yy.h"
#include "tokens.h"
//#include "hash.h"

#define MAX_FILENAME_LEN 50
#define ASCII_MIN 0
#define ASCII_MAX 255

int yylex();
extern char *yytext;
extern FILE *yyin;

int getLineNumber();
int isRunning(void);
void initMe(void);

int main(int argc, char** argv) {

	int token;
	// char[MAX_FILENAME_LEN+1] filename;

	if (argc < 2) {
		printf("compile: Missing input file.\n\nTry 'make compile filename=<filename>'\n");
		exit(1);
    }
  	if (0 == (yyin = fopen(argv[1], "r"))) {
		printf("compile: Cannot open file %s... \n", argv[1]);
		exit(1);
    }

	initMe();
	while (isRunning()) {

		token = yylex();
		if (!isRunning())
      		break;

		switch(token) {
			
			case ASCII_MIN ... ASCII_MAX:
				printf("%d: Special char %c\n", getLineNumber(), yytext[0]); break;

			case KW_CHAR:
				printf("%d: Keyword char\n", getLineNumber()); break;
			case KW_INT:
				printf("%d: Keyword int\n", getLineNumber()); break;
			case KW_FLOAT:
				printf("%d: Keyword float\n", getLineNumber()); break;

			case KW_CODE:
				printf("%d: Keyword code\n", getLineNumber()); break;

			case KW_IF:
				printf("%d: Keyword if\n", getLineNumber()); break;
			case KW_ELSE:
				printf("%d: Keyword else\n", getLineNumber()); break;
			case KW_WHILE:
				printf("%d: Keyword while\n", getLineNumber()); break;
			case KW_INPUT:
				printf("%d: Keyword input\n", getLineNumber()); break;
			case KW_PRINT:
				printf("%d: Keyword print\n", getLineNumber()); break;
			case KW_RETURN:
				printf("%d: Keyword return\n", getLineNumber()); break;

			case OPERATOR_LE:
				printf("%d: Operator <=\n", getLineNumber()); break;
			case OPERATOR_GE:
				printf("%d: Operator >=\n", getLineNumber()); break;
			case OPERATOR_EQ:
				printf("%d: Operator ==\n", getLineNumber()); break;
			case OPERATOR_DIF:
				printf("%d: Operator !=\n", getLineNumber()); break;

			case TK_IDENTIFIER:
				printf("%d: Identifier %s\n", getLineNumber(), yytext); break;

			case LIT_INT:
				printf("%d: Literal Int %s\n", getLineNumber(), yytext); break;
			case LIT_FLOAT:
				printf("%d: Literal Float %s\n", getLineNumber(), yytext); break;
			case LIT_CHAR:
				printf("%d: Literal Char %s\n", getLineNumber(), yytext); break;
			case LIT_STRING:
				printf("%d: Literal String %s\n", getLineNumber(), yytext); break;

			case TOKEN_ERROR:
				fprintf(stderr, "%d: Error %s\n", getLineNumber(), yytext); break;
		}
	}

	//hashPrint();

	return 0;
}
