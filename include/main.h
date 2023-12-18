#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>

#define ERR_NO_INPUT 101
#define ERR_OPEN_FILE 102
#define ERR_BAD_SYNTAX 3
#define ERR_SEMANTICS 4
#define ERR_INTERNAL 99

// lexer
// int yylex();
extern char *yytext;
extern FILE *yyin;

// parser
extern int yyparse();

int getLineNumber(void);
int isRunning(void);
void initMe(void);

char* get_input_filename(void);

#endif