#ifndef TOKENS_H
#define TOKENS_H

/*
 * Etapa 1 - tokens.h
 * INF-UFRGS - INF01147 Compiladores - 2023/2
 * Guilherme Klein Kern
 */

#define KW_CHAR           256
#define KW_INT            257
#define KW_FLOAT          258

#define KW_CODE           260

#define KW_IF             261
#define KW_ELSE           263
#define KW_WHILE          264
#define KW_INPUT          266
#define KW_PRINT          267
#define KW_RETURN         268

#define OPERATOR_LE       270
#define OPERATOR_GE       271
#define OPERATOR_EQ       272
#define OPERATOR_DIF      273

#define TK_IDENTIFIER     280

#define LIT_INT           281
#define LIT_FLOAT         282
#define LIT_CHAR          285
#define LIT_STRING        286

#define TOKEN_ERROR       290

#endif