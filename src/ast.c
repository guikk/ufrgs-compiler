/*
 * Etapa 4 - ast.c
 * INF-UFRGS - INF01147 Compiladores - 2023/2
 * Guilherme Klein Kern
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "ast.h"
#include "main.h"

char* ast_type_name[] = {
    "AST_PROGRAM",
    "AST_DECL_L",
    "AST_CHAR_T",
    "AST_FLOAT_T",
    "AST_INT_T",
    "AST_SYMBOL",
    "AST_DECL_VAR",
    "AST_DECL_VEC",
    "AST_DECL_FUNC",
    "AST_VECINIT_L",
    "AST_PARAM_L",
    "AST_FUNC_L",
    "AST_FUNC",
    "AST_ASSIGN",
    "AST_VECASSIGN",
    "AST_PRINT",
    "AST_RETURN",
    "AST_SCOPE",
    "AST_EMPTYCMD",
    "AST_VECELEM",
    "AST_PAREXPR",
    "AST_SUM",
    "AST_SUB",
    "AST_MUL",
    "AST_DIV",
    "AST_LT",
    "AST_GT",
    "AST_GE",
    "AST_LE",
    "AST_EQ",
    "AST_DIF",
    "AST_AND",
    "AST_OR",
    "AST_NOT",
    "AST_FUNCCALL",
    "AST_ARG_L",
    "AST_INPUT",
    "AST_STMT_L",
    "AST_IF",
    "AST_IFELSE",
    "AST_WHILE"
};

ast* ast_new(
    ast_type type,
    symbol* symbol,
    ast* c0, ast* c1, ast* c2,
    int location
) {
    ast* new = calloc(1, sizeof(ast));

    new->type = type;
    new->symbol = symbol;

    new->children[0] = c0;
    new->children[1] = c1;
    new->children[2] = c2;

    new->location = location;

    return new;
}

void _ast_print(ast* node, int level) {
    if (!node) {
        return;
    }

    int i;
    for(i = 0; i < level; i++) fprintf(stderr, "  ");

    fprintf(stderr, "AST( ");

    fprintf(stderr, "%s", ast_type_name[node->type]);

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

data_type n2dtype(ast* type_node) {
    switch (type_node->type) {
    case AST_INT_T:
        return DT_INT;
    case AST_CHAR_T:
        return DT_CHAR;
    case AST_FLOAT_T:
        return DT_FLOAT;
    default:
        fprintf(stderr,"Error: not a valid type node\n");
        exit(ERR_INTERNAL);
    }
}