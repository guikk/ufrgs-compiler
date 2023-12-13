/*
 * Etapa 4 - main.c
 * INF-UFRGS - INF01147 Compiladores - 2023/2
 * Guilherme Klein Kern
 */

#include <stdio.h>
#include "main.h"
#include "lex.yy.h"
#include "symbol_table.h"
#include "ast.h"

extern ast* get_parsed_ast();

char* input_path;

int main(int argc, char** argv) {

	if (argc < 3) {
		fprintf(stderr,"compile: Missing argument(s).\nTry 'make compile target=<input-path> out=<output-path>'\n\n");
		exit(ERR_NO_INPUT);
    }

	input_path = argv[1];
  	if (0 == (yyin = fopen(input_path, "r"))) {
		fprintf(stderr,"compile: Cannot open file \'%s\'.\n\n", input_path);
		exit(ERR_OPEN_FILE);
    }

	initMe();
	
	yyparse();

	// print_symbol_table();

	ast* parsed = get_parsed_ast();
	ast_print(parsed);
	ast_decompile(parsed, argv[2]);


	exit(0);
}

char* get_input_filename(void) {
	return input_path;
}
