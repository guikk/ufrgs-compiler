/*
 * Etapa 4 - symbol_table.c
 * INF-UFRGS - INF01147 Compiladores - 2023/2
 * Guilherme Klein Kern
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"

symbol_node* m_symbol_table[HASH_SIZE];

void init_symbol_table(void) {
    for(int i = 0; i < HASH_SIZE; i++) {
        m_symbol_table[i] = 0;
    }
}

int hash(char *text) {
    int address = 1;
    int n = strlen(text);
    for(int i = 0; i < n; i++) {
        address = (address * text[i]) % HASH_SIZE + 1;
    }
    return address - 1;
}

symbol_node* add_symbol(int type, char* text) {
    symbol_node* new = get_symbol(text);
    if(new != NULL)
        return new;

    int address = hash(text);
    new = (symbol_node*) calloc(1, sizeof(symbol_node));
    new->type = type;
    new->text = calloc(strlen(text)+1, sizeof(char));
    strcpy(new->text, text);

    new->next = m_symbol_table[address];
    m_symbol_table[address] = new;

    return new;
}

symbol_node* get_symbol(char *text) {
    int address = hash(text);
    symbol_node* node;
    for(node = m_symbol_table[address]; node != NULL; node = node->next) {
        if(strcmp(text, node->text) == 0)
            return node;
    }
    return NULL;
}

void print_symbol_table(void){
    symbol_node* node;
    printf("\n--- Symbol Table ---\n");
    for (int i = 0; i < HASH_SIZE; i++) {
        if (m_symbol_table[i] != NULL) {
            printf("Index %d = {", i);
            for (node = m_symbol_table[i]; node != NULL; node = node->next) {
                printf(" %s(%d)", node->text, node->type);
            }
            printf(" }\n");
        }
    }
}