/*
 * Etapa 2 - main.c
 * INF-UFRGS - INF01147 Compiladores - 2023/2
 * Guilherme Klein Kern
 */

#include <stdio.h>
#include "main.h"
#include "lex.yy.h"
#include "hash.h"

char* filename;

int main(int argc, char** argv) {

	if (argc < 2) {
		printf("compile: Missing input file.\nTry 'make compile target=<filename>'\n\n");
		exit(ERR_NO_INPUT);
    }

	filename = argv[1];
  	if (0 == (yyin = fopen(filename, "r"))) {
		printf("compile: Cannot open file \'%s\'.\n\n", filename);
		exit(ERR_OPEN_FILE);
    }

	initMe();
	
	yyparse();

	print_symbol_table();

	exit(0);
}

char* get_input_filename(void) {
	return filename;
}
