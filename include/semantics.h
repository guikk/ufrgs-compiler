#ifndef SEMANTICS_H
#define SEMANTICS_H

#include "ast.h"

typedef enum {
    ID_SCALAR,
    ID_VECTOR,
    ID_FUNC
} id_nature;

typedef enum {
    DT_CHAR,
    DT_INT,
    DT_FLOAT
} data_type;

int semantic_analysis(ast program);

int check_declarations(ast declaration_list);
int check_identifier_usage(ast tree);
int check_data_types(ast tree);
int check_arguments(ast tree);
int check_function(ast func);


#endif