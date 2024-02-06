/*
 * main.c
 * INF-UFRGS - INF01147 Compiladores - 2023/2
 * Guilherme Klein Kern
 */

#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "errors.h"
#include "symbol_table.h"
#include "ast.h"
#include "semantics.h"
#include "tac.h"
#include "asm.h"

// lexer
// int yylex();
extern FILE *yyin;

// parser
extern int yyparse();
extern ast* get_parsed_ast(void);

char* input_path;

char* replace_ext(char* filename, char* new_ext);

int main(int argc, char** argv) {

	if (argc < 2) {
		fprintf(stderr,"compile: Missing argument(s).\nTry 'make compile target=<input-path>'\n\n");
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
	// ast_print(parsed);

	if (argc == 3) {
		ast_decompile(parsed, argv[2]);
	}

	semantic_analysis(parsed);

	ast* declarations = parsed->children[0];
	ast* functions = parsed->children[1];

	tac* tac_list = generate_tac_list(functions);
	// print_tac_list(tac_list);

	generate_asm(declarations, tac_list, replace_ext(input_path, ".s"));

	exit(0);
}

char* get_input_filename(void) {
	return input_path;
}

char* replace_ext(char* filename, char* new_ext) {
	char* dot = strrchr(filename, '.');
	if (dot == NULL) {
		return NULL;
	}

	char* new_filename = (char*) malloc(strlen(filename) + strlen(new_ext) + 1);
	strncpy(new_filename, filename, dot - filename);
	strcpy(new_filename + (dot - filename), new_ext);
	return new_filename;
}