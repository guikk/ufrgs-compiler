/*
 * Etapa 1 - hash.c
 * INF-UFRGS - INF01147 Compiladores - 2023/2
 * Guilherme Klein Kern
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"

hash_node* m_symbol_table[HASH_SIZE];

void init_symbol_table(void) {
    for(int i = 0; i < HASH_SIZE; i++) {
        m_symbol_table[i] = 0;
    }
}

int hash(char *text) {
    int address = 1;
    for(int i = 0; i < strlen(text); i++) {
        address = (address * text[i]) % HASH_SIZE + 1;
    }
    return address - 1;
}

hash_node* add_symbol(int type, char* text) {
    hash_node* newnode = get_symbol(text);
    if(newnode != NULL)
        return newnode;

    int address = hash(text);
    newnode = (hash_node*) calloc(1, sizeof(hash_node));
    newnode->type = type;
    newnode->text = calloc(strlen(text)+1, sizeof(char));
    strcpy(newnode->text, text);

    newnode->next = m_symbol_table[address];
    m_symbol_table[address] = newnode;

    return newnode;
}

hash_node* get_symbol(char *text) {
    int address = hash(text);
    hash_node* node;
    for(node = m_symbol_table[address]; node != NULL; node = node->next) {
        if(strcmp(text, node->text) == 0)
            return node;
    }
    return NULL;
}

void print_symbol_table(void){
    hash_node* node;
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