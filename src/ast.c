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

void ast_print(ast* root) {
    _ast_print(root, 0);
}

void _ast_print(ast* node, int level) {
    if (!node) {
        return;
    }
}

void ast_decompile(ast* root, char* filename) {
    int fd = open(filename, O_WRONLY | O_TRUNC | O_CREAT, 0640);
    if (fd == -1) return;
    _ast_decompile(root, fd);
    close(fd);
}

void _ast_decompile(ast* node, int fd) {

}