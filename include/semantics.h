#ifndef SEMANTICS_H
#define SEMANTICS_H

/*
 * semantics.h
 * INF-UFRGS - INF01147 Compiladores - 2023/2
 * Guilherme Klein Kern
 */

#include "ast.h"

void semantic_analysis(ast* program);
ast* fetch_function_declaration(symbol* func_id);

#endif