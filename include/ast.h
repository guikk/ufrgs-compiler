#ifndef AST_H
#define AST_H

/*
 * Etapa 4 - ast.h
 * INF-UFRGS - INF01147 Compiladores - 2023/2
 * Guilherme Klein Kern
 */

#include "symbol_table.h"

#define MAX_CHILDREN 4

typedef enum {
    AST_PROGRAM,
    AST_DECL_L,
    AST_CHAR_T,
    AST_FLOAT_T,
    AST_INT_T,
    AST_SYMBOL,
    AST_DECL_VAR,
    AST_DECL_VEC,
    AST_DECL_FUNC,
    AST_VECINIT_L,
    AST_PARAM_L,
    AST_FUNC_L,
    AST_FUNC,
    AST_ASSIGN,
    AST_VECASSIGN,
    AST_PRINT,
    AST_RETURN,
    AST_SCOPE,
    AST_EMPTYCMD,
    AST_VECELEM,
    AST_PAREXPR,
    AST_SUM,
    AST_SUB,
    AST_MUL,
    AST_DIV,
    AST_LT,
    AST_GT,
    AST_GE,
    AST_LE,
    AST_EQ,
    AST_DIF,
    AST_AND,
    AST_OR,
    AST_NOT,
    AST_FUNCCALL,
    AST_ARG_L,
    AST_INPUT,
    AST_STMT_L,
    AST_IF,
    AST_IFELSE,
    AST_WHILE
} ast_type;

typedef struct ast {
    ast_type type;
    symbol_node* symbol;
    struct ast* children[MAX_CHILDREN];
} ast;

ast* ast_new(
    ast_type type,
    symbol_node* symbol,
    ast* c0, ast* c1, ast* c2, ast* c3
);

void ast_print(ast* root);

void ast_decompile(ast* root, char* filename);

#endif