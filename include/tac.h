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
    TAC_SYMBOL,
    TAC_MOVE,
    TAC_ADD,
    TAC_SUB,
    TAC_MUL,
    TAC_DIV,
    TAC_LT,
    TAC_GT,
    TAC_GE,
    TAC_LE,
    TAC_EQ,
    TAC_DIF,
    TAC_AND,
    TAC_OR,
    TAC_NOT,
    TAC_LABEL,
    TAC_BEGINFUN,
    TAC_ENDFUN,
    TAC_IFZ,
    TAC_JUMP,
    TAC_CALL,
    TAC_ARGPUSH,
    TAC_RET,
    TAC_PRINT,
    TAC_INPUT,
    TAC_NOP,
    TAC_VECSET,
    TAC_VECGET
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