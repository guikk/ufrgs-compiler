/*
 * Etapa 3 - ast.c
 * INF-UFRGS - INF01147 Compiladores - 2023/2
 * Guilherme Klein Kern
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "ast.h"

ast* ast_new(
    ast_type type,
    symbol_node* symbol,
    ast* c0, ast* c1, ast* c2, ast* c3
) {
    ast* new = calloc(1, sizeof(ast));

    new->type = type;
    new->symbol = symbol;

    new->children[0] = c0;
    new->children[1] = c1;
    new->children[2] = c2;
    new->children[3] = c3;

    return new;
}

void _ast_print(ast* node, int level) {
    if (!node) {
        return;
    }

    int i;
    for(i = 0; i < level; i++) fprintf(stderr, "  ");

    fprintf(stderr, "AST( ");

    switch (node->type) {
        case AST_PROGRAM: fprintf(stderr,"AST_PROGRAM"); break;
        case AST_DECL_L: fprintf(stderr,"AST_DECL_L"); break;
        case AST_CHAR_T: fprintf(stderr,"AST_CHAR_T"); break;
        case AST_FLOAT_T: fprintf(stderr,"AST_FLOAT_T"); break;
        case AST_INT_T: fprintf(stderr,"AST_INT_T"); break;
        case AST_SYMBOL: fprintf(stderr,"AST_SYMBOL"); break;
        case AST_DECL_VAR: fprintf(stderr,"AST_DECL_VAR"); break;
        case AST_DECL_VEC: fprintf(stderr,"AST_DECL_VEC"); break;
        case AST_DECL_FUNC: fprintf(stderr,"AST_DECL_FUNC"); break;
        case AST_VECINIT_L: fprintf(stderr,"AST_VECINIT_L"); break;
        case AST_PARAM_L: fprintf(stderr,"AST_PARAM_L"); break;
        case AST_FUNC_L: fprintf(stderr,"AST_FUNC_L"); break;
        case AST_FUNC: fprintf(stderr,"AST_FUNC"); break;
        case AST_ASSIGN: fprintf(stderr,"AST_ASSIGN"); break;
        case AST_VECASSIGN: fprintf(stderr,"AST_VECASSIGN"); break;
        case AST_PRINT: fprintf(stderr,"AST_PRINT"); break;
        case AST_RETURN: fprintf(stderr,"AST_RETURN"); break;
        case AST_SCOPE: fprintf(stderr,"AST_SCOPE"); break;
        case AST_EMPTYCMD: fprintf(stderr,"AST_EMPTYCMD"); break;
        case AST_VECELEM: fprintf(stderr,"AST_VECELEM"); break;
        case AST_PAREXPR: fprintf(stderr,"AST_PAREXPR"); break;
        case AST_SUM: fprintf(stderr,"AST_SUM"); break;
        case AST_SUB: fprintf(stderr,"AST_SUB"); break;
        case AST_MUL: fprintf(stderr,"AST_MUL"); break;
        case AST_DIV: fprintf(stderr,"AST_DIV"); break;
        case AST_LT: fprintf(stderr,"AST_LT"); break;
        case AST_GT: fprintf(stderr,"AST_GT"); break;
        case AST_GE: fprintf(stderr,"AST_GE"); break;
        case AST_LE: fprintf(stderr,"AST_LE"); break;
        case AST_EQ: fprintf(stderr,"AST_EQ"); break;
        case AST_DIF: fprintf(stderr,"AST_DIF"); break;
        case AST_AND: fprintf(stderr,"AST_AND"); break;
        case AST_OR: fprintf(stderr,"AST_OR"); break;
        case AST_NOT: fprintf(stderr,"AST_NOT"); break;
        case AST_FUNCCALL: fprintf(stderr,"AST_FUNCCALL"); break;
        case AST_ARG_L: fprintf(stderr,"AST_ARG_L"); break;
        case AST_INPUT: fprintf(stderr,"AST_INPUT"); break;
        case AST_STMT_L: fprintf(stderr,"AST_STMT_L"); break;
        case AST_IF: fprintf(stderr,"AST_IF"); break;
        case AST_IFELSE: fprintf(stderr,"AST_IFELSE"); break;
        case AST_WHILE: fprintf(stderr,"AST_WHILE"); break;
    }

    if(node->symbol)
        fprintf(stderr, ", %s )\n", node->symbol->text);
    else
        fprintf(stderr, " )\n");

    for(i = 0; i < MAX_CHILDREN; i++)
        _ast_print(node->children[i], level+1);
}

void ast_print(ast* root) {
    printf("\n--- Abstract Syntax Tree ---\n");
    _ast_print(root, 0);
}

void _ast_decompile(ast* node, int fd) {
    if (!node) {
        return;
    }

    switch (node->type) {
        case AST_PROGRAM:
            dprintf(fd,"// second2023 language decompiled program\n\n");
            _ast_decompile(node->children[0], fd);
            dprintf(fd,"\n");
            _ast_decompile(node->children[1], fd); break;

        case AST_DECL_L:
            _ast_decompile(node->children[0], fd);
            _ast_decompile(node->children[1], fd); break;

        case AST_CHAR_T:  dprintf(fd,"char"); break;
        case AST_FLOAT_T: dprintf(fd,"float"); break;
        case AST_INT_T:   dprintf(fd,"int"); break;
        case AST_SYMBOL:  dprintf(fd,"%s", node->symbol->text); break;

        case AST_DECL_VAR:
            _ast_decompile(node->children[0], fd);
            dprintf(fd," %s ", node->symbol->text);
            dprintf(fd,"= "); 
            _ast_decompile(node->children[1], fd);
            dprintf(fd,";\n"); break;

        case AST_DECL_VEC:
            _ast_decompile(node->children[0], fd);
            dprintf(fd," %s[", node->symbol->text);
            _ast_decompile(node->children[1], fd);
            dprintf(fd,"]");
            _ast_decompile(node->children[2], fd);
            dprintf(fd,";\n"); break;

        case AST_DECL_FUNC:
            _ast_decompile(node->children[0], fd);
            dprintf(fd," %s(", node->symbol->text);
            _ast_decompile(node->children[1], fd);
            dprintf(fd,")");
            dprintf(fd,";\n"); break;

        case AST_VECINIT_L:
            dprintf(fd," ");
            _ast_decompile(node->children[0], fd);
            _ast_decompile(node->children[1], fd); break;

        case AST_PARAM_L:
            _ast_decompile(node->children[0], fd);
            dprintf(fd," %s", node->symbol->text);
            if (node->children[1]) {
                dprintf(fd,", ");
                _ast_decompile(node->children[1], fd);
            } break;

        case AST_FUNC_L:
            _ast_decompile(node->children[0], fd);
            _ast_decompile(node->children[1], fd); break;

        case AST_FUNC:
            dprintf(fd,"code %s ", node->symbol->text);
            _ast_decompile(node->children[0], fd); break;

        case AST_ASSIGN:
            dprintf(fd,"%s = ", node->symbol->text);
            _ast_decompile(node->children[0], fd);
            dprintf(fd,";\n"); break;

        case AST_VECASSIGN:
            dprintf(fd,"%s[", node->symbol->text);
            _ast_decompile(node->children[0], fd);
            dprintf(fd,"] = ");
            _ast_decompile(node->children[1], fd);
            dprintf(fd,";\n"); break;
        
        case AST_PRINT:
            dprintf(fd,"print ");
            _ast_decompile(node->children[0], fd);
            dprintf(fd,";\n"); break;

        case AST_RETURN: 
            dprintf(fd,"return ");
            _ast_decompile(node->children[0], fd);
            dprintf(fd,";\n"); break;

        case AST_SCOPE:
            dprintf(fd,"{\n");
            _ast_decompile(node->children[0], fd);
            dprintf(fd,"}\n"); break;

        case AST_EMPTYCMD: dprintf(fd,";\n"); break;

        case AST_VECELEM:
            dprintf(fd,"%s[", node->symbol->text);
            _ast_decompile(node->children[0], fd);
            dprintf(fd,"]"); break;
        
        case AST_PAREXPR:
            dprintf(fd,"(");
            _ast_decompile(node->children[0], fd);
            dprintf(fd,")"); break;

        case AST_SUM:
            _ast_decompile(node->children[0], fd);
            dprintf(fd," + ");
            _ast_decompile(node->children[1], fd); break;

        case AST_SUB:
            _ast_decompile(node->children[0], fd);
            dprintf(fd," - ");
            _ast_decompile(node->children[1], fd); break;

        case AST_MUL: 
            _ast_decompile(node->children[0], fd);
            dprintf(fd," + ");
            _ast_decompile(node->children[1], fd); break;
            
        case AST_DIV: 
            _ast_decompile(node->children[0], fd);
            dprintf(fd," / ");
            _ast_decompile(node->children[1], fd); break;
        
        case AST_LT: 
            _ast_decompile(node->children[0], fd);
            dprintf(fd," < ");
            _ast_decompile(node->children[1], fd); break;
        
        case AST_GT: 
            _ast_decompile(node->children[0], fd);
            dprintf(fd," > ");
            _ast_decompile(node->children[1], fd); break;
        
        case AST_GE: 
            _ast_decompile(node->children[0], fd);
            dprintf(fd," >= ");
            _ast_decompile(node->children[1], fd); break;
        
        case AST_LE: 
            _ast_decompile(node->children[0], fd);
            dprintf(fd," <= ");
            _ast_decompile(node->children[1], fd); break;
        
        case AST_EQ: 
            _ast_decompile(node->children[0], fd);
            dprintf(fd," == ");
            _ast_decompile(node->children[1], fd); break;
        
        case AST_DIF: 
            _ast_decompile(node->children[0], fd);
            dprintf(fd," != ");
            _ast_decompile(node->children[1], fd); break;
        
        case AST_AND: 
            _ast_decompile(node->children[0], fd);
            dprintf(fd," & ");
            _ast_decompile(node->children[1], fd); break;
        
        case AST_OR: 
            _ast_decompile(node->children[0], fd);
            dprintf(fd," | ");
            _ast_decompile(node->children[1], fd); break;
        
        case AST_NOT:
            dprintf(fd,"~");
            _ast_decompile(node->children[0], fd); break;

        case AST_FUNCCALL:
            dprintf(fd," %s(", node->symbol->text);
            _ast_decompile(node->children[0], fd);
            dprintf(fd,")"); break;
        
        case AST_ARG_L:
            _ast_decompile(node->children[0], fd);
            if (node->children[1]) {
                dprintf(fd,", ");
                _ast_decompile(node->children[1], fd);
            } break;

        case AST_INPUT:
            dprintf(fd,"input(");
            _ast_decompile(node->children[0], fd);
            dprintf(fd,")"); break;

        case AST_STMT_L:
            _ast_decompile(node->children[0], fd);
            _ast_decompile(node->children[1], fd); break;
        
        case AST_IF:
            dprintf(fd,"if (");
            _ast_decompile(node->children[0], fd);
            dprintf(fd,") ");
            _ast_decompile(node->children[1], fd); break;

        case AST_IFELSE:
            dprintf(fd,"if (");
            _ast_decompile(node->children[0], fd);
            dprintf(fd,") ");
            _ast_decompile(node->children[1], fd);
            dprintf(fd,"else ");
            _ast_decompile(node->children[2], fd); break;

        case AST_WHILE:
            dprintf(fd,"while (");
            _ast_decompile(node->children[0], fd);
            dprintf(fd,") ");
            _ast_decompile(node->children[1], fd); break;
    }
}

void ast_decompile(ast* root, char* filename) {
    int fd = open(filename, O_WRONLY | O_TRUNC | O_CREAT, 0640);
    if (fd == -1) {
        fprintf(stderr,"compile: Warning - error opening output file.\n\n");
        return;
    }
    _ast_decompile(root, fd);
    close(fd);
}