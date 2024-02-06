#ifndef ASM_H
#define ASM_H

/*
 * asm.h
 * INF-UFRGS - INF01147 Compiladores - 2023/2
 * Guilherme Klein Kern
 */

#include "ast.h"
#include "tac.h"

void generate_asm(ast* decl_list, tac* tac_list, char *output_file);

#endif