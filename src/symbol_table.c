/*
 * Etapa 4 - symbol_table.c
 * INF-UFRGS - INF01147 Compiladores - 2023/2
 * Guilherme Klein Kern
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"

symbol* m_symbol_table[HASH_SIZE];

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

symbol* add_symbol(symbol_type type, char* text) {
    symbol* new = get_symbol(text);
    if(new != NULL)
        return new;

    int address = hash(text);
    new = (symbol*) calloc(1, sizeof(symbol));
    new->dtype = DT_UNDEFINED;
    new->stype = type;
    new->text = calloc(strlen(text)+1, sizeof(char));
    strcpy(new->text, text);

    switch (type) {
    case SYMBOL_LIT_CHAR:  new->dtype = DT_CHAR; break;
    case SYMBOL_LIT_INT:   new->dtype = DT_INT; break;
    case SYMBOL_LIT_FLOAT: new->dtype = DT_FLOAT; break;
    default: break;
    }

    new->next = m_symbol_table[address];
    m_symbol_table[address] = new;

    return new;
}

symbol* get_symbol(char *text) {
    int address = hash(text);
    symbol* node;
    for(node = m_symbol_table[address]; node != NULL; node = node->next) {
        if(strcmp(text, node->text) == 0)
            return node;
    }
    return NULL;
}

void print_symbol_table(void){
    symbol* node;
    printf("\n--- Symbol Table ---\n");
    for (int i = 0; i < HASH_SIZE; i++) {
        if (m_symbol_table[i] != NULL) {
            printf("Index %d = {", i);
            for (node = m_symbol_table[i]; node != NULL; node = node->next) {
                printf(" %s(%d)", node->text, node->stype);
            }
            printf(" }\n");
        }
    }
}