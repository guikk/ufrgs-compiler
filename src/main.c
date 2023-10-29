/*
 * Etapa 2 - main.c
 * INF-UFRGS - INF01147 Compiladores - 2023/2
 * Guilherme Klein Kern
 */

#include <stdio.h>
#include "main.h"
#include "lex.yy.h"
#include "tokens.h"
#include "hash.h"

int main(int argc, char** argv) {

	int token;

	if (argc < 2) {
		printf("compile: Missing input file.\nTry 'make compile target=<filename>'\n\n");
		exit(ERR_NO_INPUT);
    }
  	if (0 == (yyin = fopen(argv[1], "r"))) {
		printf("compile: Cannot open file \'%s\'.\n\n", argv[1]);
		exit(ERR_OPEN_FILE);
    }

	initMe();
	
	yyparse();

	print_symbol_table();

	exit(0);
}
