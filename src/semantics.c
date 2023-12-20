/*
 * Etapa 4 - semantics.c
 * INF-UFRGS - INF01147 Compiladores - 2023/2
 * Guilherme Klein Kern
 */

#include <stdlib.h>
#include <stdarg.h>

#include "semantics.h"
#include "main.h"
#include "ast.h"

void check_declarations(ast* decl_list);
void check_declaration(ast* declaration);
void register_identifier(id_nature nature, symbol* identifier, ast* type_kw);

void check_type_compatibility(data_type a, data_type b);

void check_functions(ast* func_list);
void check_function(ast* func);

void check_missing_implementations(ast* decl_list);

void check_statement(ast* statement, data_type return_type);

int check_identifier_usage(symbol* id, id_nature nature);

data_type check_expression(ast* expr);

void check_data_types(ast* tree);
void check_arguments(ast* tree);

int m_location; // line where the AST node being analyzed is on the source code
int m_error_count = 0;

void semantic_analysis(ast* program) {
    if (!program || program->type != AST_PROGRAM) {
        fprintf(stderr,"Error: not a valid program");
        exit(ERR_INTERNAL);
    }

    ast* declarations = program->children[0];
    ast* functions = program->children[1];

    check_declarations(declarations);
    check_functions(functions);

    check_missing_implementations(declarations);

    if (m_error_count > 0) {  
        fprintf(stderr,"%s: found %d error(s)\n", get_input_filename(), m_error_count);
        exit(ERR_SEMANTICS);
    }
}

/*
 * display to standard error output a formatted error message
 * and increment error count
 */
void semantic_error(char* message, ...) {
    va_list args;
    va_start(args, message);
    fprintf(stderr,"%s:%d: ", get_input_filename(), m_location);
    vfprintf(stderr, message, args);
    fprintf(stderr,"\n");
    va_end(args);
    m_error_count++;
}

/*
 * decorate identifiers: nature, data type
 * check for duplicated declarations
 * check initialization: type, size
 */
void check_declarations(ast* decl_list) {
    if (!decl_list) {
        return;
    }

    if (decl_list->type != AST_DECL_L) {
        fprintf(stderr,"Error: not a valid declaration list\n");
        exit(ERR_INTERNAL);
    }

    check_declaration(decl_list->children[0]);
    check_declarations(decl_list->children[1]);
}

void check_declaration(ast* declaration) {
    if (!declaration) {
        fprintf(stderr,"Error: null declaration\n");
        exit(ERR_INTERNAL);
    }

    m_location = declaration->location;
    symbol* id = declaration->symbol;
    ast* type_kw = declaration->children[0];

    switch(declaration->type) {
    case AST_DECL_VAR:
        register_identifier(ID_SCALAR, id, type_kw);
        
        ast* init = declaration->children[1];
        if (!init) {
            break;
        }

        check_type_compatibility(
            id->dtype,
            init->symbol->dtype
        );
        break;
    case AST_DECL_VEC: 
        register_identifier(ID_VECTOR, id, type_kw);

        ast* vec_size = declaration->children[1];
        ast* vec_init = declaration->children[2];

        if (!vec_init) {
            return;
        }

        // check initialization type and size
        int expected = atoi(vec_size->symbol->text);
        int actual = 0;
        while(vec_init) {
            actual++;
            check_type_compatibility(
                id->dtype,
                vec_init->children[0]->symbol->dtype
            );
            vec_init = vec_init->children[1];
        }
        
        // check initialization size
        if (expected != actual) {
            semantic_error("expected %d vector elements but got %d", expected, actual);
        }
        break;
    case AST_DECL_FUNC: 
        register_identifier(ID_FUNC, id, type_kw);
        id->is_implemented = 0;

        // decorate function argument identifiers
        ast* param_list = declaration->children[1];
        while (param_list) {
            register_identifier(
                ID_SCALAR,
                param_list->symbol,
                param_list->children[0]
            );
            param_list = param_list->children[1];
        }
        break;
    default:
        fprintf(stderr,"Error: not a declaration\n");
        exit(ERR_INTERNAL);
    }
}

void register_identifier(id_nature nature, symbol* identifier, ast* type_kw) {
    if (identifier->stype != SYMBOL_IDENTIFIER) {
        fprintf(stderr,"Error: not an identifier");
        exit(ERR_INTERNAL);
    } 
    
    if (identifier->dtype != DT_UNDEFINED) {
        semantic_error(
            "identifier '%s' already defined",
            identifier->text
        );
    }

    identifier->nature = nature;
    switch(type_kw->type) {
    case AST_INT_T:
        identifier->dtype = DT_INT;
        break;
    case AST_CHAR_T:
        identifier->dtype = DT_CHAR;
        break;
    case AST_FLOAT_T:
        identifier->dtype = DT_FLOAT;
        break;
    default:
        fprintf(stderr,"Error: not a valid type node");
        exit(ERR_INTERNAL);
    }
}

char* dt_str[] = {
    "undefined",
    "char",
    "int",
    "float"
};

void check_type_compatibility(data_type a, data_type b) {
    if (a == b) {
        return;
    }

    if (a == DT_FLOAT || b == DT_FLOAT) {
        semantic_error(
            "type %s is not compatible with %s",
            dt_str[a], dt_str[b]
        );
    }
}

void check_functions(ast* func_list) {
    if (!func_list) {
        return;
    }

    if (func_list->type != AST_FUNC_L) {
        fprintf(stderr,"Error: not a valid function list\n");
        exit(ERR_INTERNAL);
    }

    check_function(func_list->children[0]);
    check_functions(func_list->children[1]);
}

void check_function(ast* function) {
     if (!function || function->type != AST_FUNC) {
        fprintf(stderr,"Error: not a valid function\n");
        exit(ERR_INTERNAL);
    }

    m_location = function->location;
    symbol* id = function->symbol;

    check_identifier_usage(id, ID_FUNC);

    check_statement(function->children[0], id->dtype);

    id->is_implemented = 1;
}

void check_missing_implementations(ast* decl_list) {
    while(decl_list) {
        ast* decl = decl_list->children[0];

        if (decl->type == AST_DECL_FUNC && !decl->symbol->is_implemented) {
            semantic_error("function '%s' declared and not implemented", decl->symbol->text);
        }

        decl_list = decl_list->children[1];
    }
}

void check_statement(ast* statement, data_type return_type) {
    if (!statement) {
        fprintf(stderr,"Error: null statement\n");
        exit(ERR_INTERNAL);
    }

    m_location = statement->location;
    data_type dt;

    switch (statement->type) {
    case AST_IFELSE:
        check_statement(statement->children[2], return_type);
    case AST_IF:
    case AST_WHILE:
        if (check_expression(statement->children[0]) != DT_INT) {
            semantic_error("condition must be integer");
        }
        check_statement(statement->children[1], return_type);
        break;

    // commands
    case AST_ASSIGN:
        check_identifier_usage(statement->symbol, ID_SCALAR);
        dt = check_expression(statement->children[1]);
        check_type_compatibility(statement->symbol->dtype, dt);
        break;
    case AST_VECASSIGN:
        check_identifier_usage(statement->symbol, ID_VECTOR);
        if (check_expression(statement->children[0]) != DT_INT) {
            semantic_error("vector index must be an integer");
        }
        
        dt = check_expression(statement->children[0]);
        check_type_compatibility(statement->symbol->dtype, dt);
        break;

    case AST_PRINT:
        ast* arg = statement->children[0];
        if (arg->type == AST_SYMBOL && arg->symbol->stype == SYMBOL_LIT_STRING) {
            break;
        }
        check_expression(arg);
        break;
    case AST_RETURN:
        check_type_compatibility(
            return_type,
            check_expression(statement->children[0])
        );
        // if (dt != return_type) {
        //     semantic_error(
        //         "expected %s as return type but got %s",
        //         dt_str[return_type], dt_str[dt]
        //     );
        // }
        break;
    case AST_SCOPE:
        ast* stmt_list = statement->children[0];
        while(stmt_list) {
            check_statement(stmt_list->children[0], return_type);
            stmt_list = stmt_list->children[1];
        }
        break;
    case AST_EMPTYCMD:
        break;
    default:
        fprintf(stderr,"Error: not a valid statement");
        exit(ERR_INTERNAL);
    }

}

char* nature_str[] = {
    "undefined",
    "scalar",
    "vector",
    "function"
};

int check_identifier_usage(symbol* id, id_nature nature) {
    if (id->nature == ID_UNDEFINED) {
        semantic_error("identifier '%s' undefined", id->text);
        return 1;
    }

    if (id->nature != nature) {
        semantic_error(
            "'%s' (%s) is not a %s identifier",
            id->text, nature_str[id->nature], nature_str[nature]
        );
        return 2;
    }

    return 0;
}

data_type check_expression(ast* expr) {

    return DT_UNDEFINED;
}