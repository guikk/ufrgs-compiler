#ifndef HASH_H
#define HASH_H

/*
 * Etapa 1 - hash.h
 * INF-UFRGS - INF01147 Compiladores - 2023/2
 * Guilherme Klein Kern
 */

#define HASH_SIZE 997

#define SYMBOL_LIT_INT 1
#define SYMBOL_LIT_FLOAT 2
#define SYMBOL_LIT_CHAR 3
#define SYMBOL_LIT_STRING 4
#define SYMBOL_IDENTIFIER 5

typedef struct hash_node{
    int type;
    char* text;
    struct hash_node* next;
} hash_node;

int hash(char *text);
hash_node* add_symbol(int type, char* text);
hash_node* get_symbol(char *text);
void init_symbol_table(void);
void print_symbol_table(void);

#endif