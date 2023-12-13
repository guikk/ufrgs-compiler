#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

/*
 * Etapa 4 - symbol_table.h
 * INF-UFRGS - INF01147 Compiladores - 2023/2
 * Guilherme Klein Kern
 */

#define HASH_SIZE 997

#define SYMBOL_LIT_INT 1
#define SYMBOL_LIT_FLOAT 2
#define SYMBOL_LIT_CHAR 3
#define SYMBOL_LIT_STRING 4
#define SYMBOL_IDENTIFIER 5

typedef struct symbol_node{
    int type;
    char* text;
    struct symbol_node* next;
} symbol_node;

int hash(char *text);
symbol_node* add_symbol(int type, char* text);
symbol_node* get_symbol(char *text);
void init_symbol_table(void);
void print_symbol_table(void);

#endif
