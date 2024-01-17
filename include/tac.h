#ifndef TAC_H
#define TAC_H

/*
 * Etapa 5 - tac.h
 * INF-UFRGS - INF01147 Compiladores - 2023/2
 * Guilherme Klein Kern
 */

#include "ast.h"
#include "symbol_table.h"

typedef enum {
    TAC_NOP,
    TAC_SYMBOL,
    TAC_ADD,
    TAC_SUB,
    TAC_MUL,
    TAC_DIV
} opcode;

typedef struct tac_node {
    opcode op;
    symbol* arg1;
    symbol* arg2;
    symbol* result;
    struct tac_node* prev;
    struct tac_node* next;
} tac;

tac* generate_tac_list(ast* program);
void print_tac_list(tac* tac_list);

#endif