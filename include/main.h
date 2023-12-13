#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>

#define ASCII_MIN 0
#define ASCII_MAX 255

#define ERR_NO_INPUT 1
#define ERR_OPEN_FILE 2
#define ERR_BAD_SYNTAX 3

// lexer
// int yylex();
extern char *yytext;
extern FILE *yyin;

// parser
extern int yyparse();

int getLineNumber();
int isRunning(void);
void initMe(void);

char* get_input_filename(void);

#endif