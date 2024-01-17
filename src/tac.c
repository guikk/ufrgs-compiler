/*
 * Etapa 5 - tac.c
 * INF-UFRGS - INF01147 Compiladores - 2023/2
 * Guilherme Klein Kern
 */

#include "tac.h"
#include <stdlib.h>

tac* tac_create(ast* node);
tac* tac_join(tac* tac1, tac* tac2);

tac* generate_tac_list(ast* program) {
    return tac_create(program);
}

tac* tac_create(ast* node) {
    if (node == NULL) {
        return NULL;
    }

    tac* child_code[MAX_CHILDREN];
    for (int i = 0; i < MAX_CHILDREN; i++) {
        child_code[i] = generate_tac(node->children[i]);
    }

    tac* tac_node = NULL;
    switch (node->type) {
        case AST_PROGRAM: break;
        case AST_DECL_L: break; 
        case AST_CHAR_T: break;
        case AST_FLOAT_T: break;      
        case AST_INT_T: break;
    }

    return tac_node;
}

void print_tac_list(tac* tac_list) {
    tac* current = tac_list;
    while (current != NULL) {
        printf("op: %d\n", current->op);
        printf("arg1: %s\n", current->arg1->text);
        printf("arg2: %s\n", current->arg2->text);
        printf("result: %s\n", current->result->text);
        printf("\n");
        current = current->next;
    }
}