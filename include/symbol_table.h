#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

/*
 * Etapa 4 - symbol_table.h
 * INF-UFRGS - INF01147 Compiladores - 2023/2
 * Guilherme Klein Kern
 */

#include "semantics.h"

#define HASH_SIZE 997

typedef enum {
    SYMBOL_LIT_INT,
    SYMBOL_LIT_FLOAT,
    SYMBOL_LIT_CHAR,
    SYMBOL_LIT_STRING,
    SYMBOL_IDENTIFIER
} symbol_type;

typedef struct symbol_node{
    symbol_type stype;
    data_type dtype;
    id_nature nature;
    char* text;
    struct symbol_node* next;
} symbol_node;

int hash(char *text);
symbol_node* add_symbol(symbol_type type, char* text);
symbol_node* get_symbol(char *text);
void init_symbol_table(void);
void print_symbol_table(void);

#endif
