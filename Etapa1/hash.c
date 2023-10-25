/* 
	ETAPA 1 - Compiladores - 2019/1 Turma A - Prof. Marcelo Johann
	Alunos:
		Guilherme Haetinger e Lucas Alegre 
*/

#include "hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void hashInit(void) {
    for(int i = 0; i < HASH_SIZE; i++) {
        Table[i] = 0;
    }
}

hash_node* hashInsert(int type, char* text) {
    hash_node* newnode = hashFind(text);
    if(newnode != NULL)
        return newnode;

    int address = hashAddress(text);
    newnode = (hash_node*) calloc(1, sizeof(hash_node));
    newnode->type = type;
    newnode->text = calloc(strlen(text) + 1, sizeof(char));
    strcpy(newnode->text, text);

    newnode->next = Table[address];
    Table[address] = newnode;

    return newnode;
}

hash_node* hashFind(char *text) {
    int address = hashAddress(text);
    hash_node* node;
    for(node = Table[address]; node != NULL; node = node->next) {
        if(strcmp(text, node->text) == 0)
            return node;
    }
    return NULL;
}

int hashAddress(char *text) {
    int address = 1;
    for(int i = 0; i < strlen(text); i++) {
        address = (address * text[i]) % HASH_SIZE + 1;
    }
    return address - 1;
}

void hashPrint(void){
    hash_node* node;
    for(int i = 0; i < HASH_SIZE; i++) {
        if(Table[i] != NULL) {
            for(node = Table[i]; node != NULL; node = node->next) {
                printf("Table[%d] - type: %d text: %s\n", i, node->type, node->text);
            }
        }
    }
}