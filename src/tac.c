/*
 * Etapa 5 - tac.c
 * INF-UFRGS - INF01147 Compiladores - 2023/2
 * Guilherme Klein Kern
 */

#include "tac.h"
#include "main.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

char* opcode_str[] = {
    "TAC_SYMBOL",
    "TAC_MOVE",
    "TAC_ADD",
    "TAC_SUB",
    "TAC_MUL",
    "TAC_DIV",
    "TAC_LT",
    "TAC_GT",
    "TAC_GE",
    "TAC_LE",
    "TAC_EQ",
    "TAC_DIF",
    "TAC_AND",
    "TAC_OR",
    "TAC_NOT",
    "TAC_LABEL",
    "TAC_BEGINFUN",
    "TAC_ENDFUN",
    "TAC_IFZ",
    "TAC_JUMP",
    "TAC_CALL",
    "TAC_ARGPUSH",
    "TAC_RET",
    "TAC_PRINT",
    "TAC_INPUT",
    "TAC_NOP",
    "TAC_VECSET",
    "TAC_VECGET"
};

void tac_print(tac* code);
tac* tac_new(opcode op, symbol* arg1, symbol* arg2, symbol* result);
tac* tac_create(ast* node);
tac* tac_first(tac* tac_list);
tac* tac_join_n(int n, ...);
tac* tac_join(tac* tac1, tac* tac2);
tac* tac_binop(opcode op, tac* tac1, tac* tac2);
tac* tac_if(tac* cond, tac* then_code);
tac* tac_while(tac* cond, tac* loop_code);
tac* tac_ifelse(tac* cond, tac* then_code, tac* else_code);
tac* tac_input(ast* input);


void print_tac_list(tac* tac_list) {
    tac* current = tac_first(tac_list);

    while (current != NULL) {
        tac_print(current);
        current = current->next;
    }
}

tac* generate_tac_list(ast* program) {
    tac* tac_list;
    tac_list = tac_create(program);
    if (tac_list == NULL) {
        fprintf(stderr,"\033[0;31m%s: tac generation failed :(\n\033[0m", get_input_filename());
        exit(ERR_INTERNAL);
    }
    fprintf(stderr,"\033[0;32m%s: tac generated successfully :)\n\033[0m", get_input_filename());
    return tac_list;
}

////////////////////////////////////////////////////////////////////////

void tac_print(tac* code) {
    if (code->op == TAC_SYMBOL) {
        return;
    }

    fprintf(stderr, "%s(", opcode_str[code->op]);
    if (code->arg1 != NULL) fprintf(stderr, "%s", code->arg1->text);
    if (code->arg2 != NULL) fprintf(stderr, ", %s", code->arg2->text);
    fprintf(stderr, ")");
    if (code->result != NULL)
        fprintf(stderr, " -> %s\n", code->result->text);
    else
        fprintf(stderr, "\n");
}

tac* tac_new(opcode op, symbol* arg1, symbol* arg2, symbol* result) {
    tac* tac_node = (tac*) malloc(sizeof(tac));
    tac_node->op = op;
    tac_node->arg1 = arg1;
    tac_node->arg2 = arg2;
    tac_node->result = result;
    tac_node->prev = NULL;
    tac_node->next = NULL;
    // tac_print(tac_node);
    return tac_node;
}

tac* tac_create(ast* tree) {
    if (tree == NULL) {
        return NULL;
    }
    
    if (tree->type == AST_DECL_L) {
        return NULL;
    }

    tac* c[MAX_CHILDREN] = {};
    for (int i = 0; i < MAX_CHILDREN; i++) {
        if (tree->children[i] == NULL) {
            continue;
        }

        c[i] = tac_create(tree->children[i]);
        if (c[i] == NULL && tree->children[i]->type >= AST_SYMBOL) {
            fprintf(stderr, "Error creating tac for node %d\n", tree->children[i]->type);
            exit(ERR_INTERNAL);
        }
    }

    switch (tree->type) {
    // Control flow
    case AST_IF: return tac_if(c[0],c[1]);
    case AST_WHILE: return tac_while(c[0],c[1]);
    case AST_IFELSE: return tac_ifelse(c[0],c[1],c[2]);

    // Commands
    case AST_ASSIGN:
        return tac_join(c[0], tac_new(
            TAC_MOVE,
            c[0]->result,
            NULL,
            tree->symbol
        ));
    case AST_VECASSIGN:
        return tac_join_n(3, c[0], c[1], tac_new(
            TAC_VECSET,
            c[1]->result, // value
            c[0]->result, // index
            tree->symbol
        ));
    case AST_PRINT: return tac_join(c[0], tac_new(TAC_PRINT, c[0]->result, NULL, NULL));
    case AST_RETURN: return tac_join(c[0], tac_new(TAC_RET, c[0]->result, NULL, NULL));
    case AST_SCOPE: return c[0] ? c[0] : tac_new(TAC_NOP, NULL, NULL, NULL);
    case AST_EMPTYCMD: return tac_new(TAC_NOP, NULL, NULL, NULL);

    // Expressions
    case AST_VECELEM:
        return tac_join(c[0], tac_new(
            TAC_VECGET,
            tree->symbol,
            c[0]->result,
            symbol_create_temp()
        ));
    case AST_FUNCCALL:
        return tac_join(c[0], tac_new(
            TAC_CALL,
            tree->symbol,
            NULL,
            symbol_create_temp()
        ));

    // Binary operations
    case AST_SUM: return tac_binop(TAC_ADD, c[0], c[1]);
    case AST_SUB: return tac_binop(TAC_SUB, c[0], c[1]);
    case AST_MUL: return tac_binop(TAC_MUL, c[0], c[1]);
    case AST_DIV: return tac_binop(TAC_DIV, c[0], c[1]);
    case AST_LT: return tac_binop(TAC_LT, c[0], c[1]);
    case AST_GT: return tac_binop(TAC_GT, c[0], c[1]);
    case AST_GE: return tac_binop(TAC_GE, c[0], c[1]);
    case AST_LE: return tac_binop(TAC_LE, c[0], c[1]);
    case AST_EQ: return tac_binop(TAC_EQ, c[0], c[1]);
    case AST_DIF: return tac_binop(TAC_DIF, c[0], c[1]);
    case AST_AND: return tac_binop(TAC_AND, c[0], c[1]);
    case AST_OR: return tac_binop(TAC_OR, c[0], c[1]);
    
    // Unary operations
    case AST_PAREXPR: return c[0];
    case AST_NOT: return tac_binop(TAC_NOT, c[0], NULL);

    // Leaf nodes
    case AST_SYMBOL: return tac_new(TAC_SYMBOL, NULL, NULL, tree->symbol);
    case AST_INPUT: return tac_input(tree);

    // Lists
    case AST_DECL_L:
    case AST_FUNC_L:
    case AST_STMT_L:
        return tac_join(c[0], c[1]);

    // Special cases
    case AST_ARG_L: // arg list is reversed so it can be used as a stack
        return tac_join_n(3, c[1], c[0], tac_new(TAC_ARGPUSH, c[0]->result, NULL, NULL));

    case AST_PROGRAM:
        return tac_join(c[0], c[1]);

    case AST_FUNC: // function implementation
        return tac_join_n(3,
            tac_new(TAC_BEGINFUN, tree->symbol, NULL, NULL),
            c[0],
            tac_new(TAC_ENDFUN, tree->symbol, NULL, NULL)
        );

    case AST_CHAR_T:
    case AST_FLOAT_T:
    case AST_INT_T:
        return NULL; // no code generated for type declarations

    default:
        fprintf(stderr, "Warning: creating default tac for node %d\n", tree->type);
        return tac_join_n(3, c[0], c[1], c[2]);
    }
}

tac* tac_first(tac* tac_list) {
    if (tac_list == NULL) {
        return NULL;
    }

    tac* current = tac_list;

    while (current->prev != NULL) {
        current = current->prev;
    }

    return current;
}

tac* tac_join_n(int n, ...) {
    va_list ap;
    tac* tac2;
    tac* current = NULL;

    va_start(ap, n);
    for (int i = 0; i < n; i++) {
        tac2 = va_arg(ap, tac*);
        current = tac_join(current, tac2);
    }
    va_end(ap);

    return current;
}

tac* tac_join(tac* tac1, tac* tac2) {
    if (tac1 == NULL) {
        return tac2;
    }
    if (tac2 == NULL) {
        return tac1;
    }

    tac* current = tac_first(tac2);
    current->prev = tac1;
    tac1->next = current;

    return tac2;
}

tac* tac_binop(opcode op, tac* arg1, tac* arg2) {
    symbol* result = symbol_create_temp();
    return tac_join_n(3,
        arg1,
        arg2,
        tac_new(
            op,
            arg1->result,
            arg2 ? arg2->result : 0, // support unary operations
            result
        )
    );
}

tac* tac_if(tac* cond, tac* then_code) {
    symbol* label = symbol_create_label();
    return tac_join_n(4,
        cond,
        tac_new(TAC_IFZ, cond->result, NULL, label),
        then_code,
        tac_new(TAC_LABEL, label, NULL, NULL)
    );
}

tac* tac_while(tac* cond, tac* loop) {
    symbol* loop_label = symbol_create_label();
    symbol* end_label = symbol_create_label();
    return tac_join_n(6,
        tac_new(TAC_LABEL, loop_label, NULL, NULL),
        cond,
        tac_new(TAC_IFZ, cond->result, NULL, end_label),
        loop,
        tac_new(TAC_JUMP, NULL, NULL, loop_label),
        tac_new(TAC_LABEL, end_label, NULL, NULL)
    );
}

tac* tac_ifelse(tac* cond, tac* then_code, tac* else_code) {
    symbol* else_label = symbol_create_label();
    symbol* end_label = symbol_create_label();
    return tac_join_n(7,
        cond,
        tac_new(TAC_IFZ, cond->result, NULL, else_label),
        then_code,
        tac_new(TAC_JUMP, NULL, NULL, end_label),
        tac_new(TAC_LABEL, else_label, NULL, NULL),
        else_code,
        tac_new(TAC_LABEL, end_label, NULL, NULL)
    );
}

tac* tac_input(ast* input) {
    symbol* temp = symbol_create_temp();
    temp->dtype = n2dtype(input->children[0]);
    return tac_new(TAC_INPUT, NULL, NULL, temp);
}