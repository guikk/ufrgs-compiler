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

int check_type_compatibility(data_type a, data_type b);

void check_functions(ast* func_list);
void check_function(ast* func);

void check_missing_implementations(ast* decl_list);

void check_statement(ast* statement, data_type return_type);

int check_identifier_usage(symbol* id, id_nature nature);

data_type check_expression(ast* expr);

ast* fetch_function_declaration(symbol* func_id);
void check_arguments(symbol* func_id, ast* arg_list);

int m_location; // line where the AST node being analyzed is on the source code
int m_error_count = 0;
ast* m_declarations;

void semantic_analysis(ast* program) {
    if (!program || program->type != AST_PROGRAM) {
        fprintf(stderr,"Error: not a valid program\n");
        exit(ERR_INTERNAL);
    }

    ast* declarations = program->children[0];
    ast* functions = program->children[1];

    m_declarations = declarations;
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
    int err;

    switch (declaration->type) {
    case AST_DECL_VAR:
        register_identifier(ID_SCALAR, id, type_kw);
        
        ast* init = declaration->children[1];
        if (!init) {
            break;
        } 
        
        err = check_type_compatibility(
            id->dtype,
            init->symbol->dtype
        );
        if (err) {
            semantic_error(
                "can't initialize %s variable '%s' with %s literal",
                dt_str(id->dtype), id->text, dt_str(init->symbol->dtype)
            );
        }
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
            data_type elem_dt = vec_init->children[0]->symbol->dtype;
            err = check_type_compatibility(id->dtype, elem_dt);
            if (err) {
                semantic_error(
                    "can't initialize %s vector '%s' with %s literal",
                    dt_str(id->dtype), id->text, dt_str(elem_dt)
                );
            }
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

void register_identifier(id_nature nature, symbol* identifier, ast* type_kw) {
    if (identifier->stype != SYMBOL_IDENTIFIER) {
        fprintf(stderr,"Error: not an identifier\n");
        exit(ERR_INTERNAL);
    } 
    
    if (identifier->dtype != DT_UNDEFINED) {
        semantic_error(
            "identifier '%s' already defined",
            identifier->text
        );
    }

    identifier->nature = nature;
    identifier->dtype = n2dtype(type_kw);
}

int check_type_compatibility(data_type a, data_type b) {
    if (
        (a == b) ||
        (a==DT_INT && b==DT_CHAR) ||
        (a==DT_CHAR && b==DT_INT)
    ) {
        return 0;
    }

    return 1;
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
    int err;
    symbol* id;

    switch (statement->type) {
    case AST_IF:
    case AST_WHILE:
        if (check_expression(statement->children[0]) != DT_INT) {
            semantic_error("condition must be integer");
        }
        check_statement(statement->children[1], return_type);
        break;
    case AST_IFELSE:
        if (check_expression(statement->children[0]) != DT_INT) {
            semantic_error("condition must be integer");
        }
        check_statement(statement->children[1], return_type);
        check_statement(statement->children[2], return_type);
        break;

    // commands
    case AST_ASSIGN:
        id = statement->symbol;
        check_identifier_usage(id, ID_SCALAR);
        dt = check_expression(statement->children[0]);
        err = check_type_compatibility(id->dtype, dt);
        if (err) {
            semantic_error(
                "can't assign %s expression to %s variable '%s'",
                dt_str(dt), dt_str(id->dtype), id->text
            );
        }
        break;
    case AST_VECASSIGN:
        id = statement->symbol;
        check_identifier_usage(id, ID_VECTOR);
        if (check_expression(statement->children[0]) != DT_INT) {
            semantic_error("vector index must be an integer");
        }
        
        dt = check_expression(statement->children[1]);
        err = check_type_compatibility(id->dtype, dt);
        if (err) {
            semantic_error(
                "can't assign %s expression to %s vector '%s'",
                dt_str(dt), dt_str(id->dtype), id->text
            );
        }
        break;

    case AST_PRINT:
        ast* arg = statement->children[0];
        if (arg->type == AST_SYMBOL && arg->symbol->stype == SYMBOL_LIT_STRING) {
            break;
        }
        check_expression(arg);
        break;
    case AST_RETURN:
        dt = check_expression(statement->children[0]);
        err = check_type_compatibility(return_type, dt);
        if (err) {
            semantic_error(
                "expected %s as return type but got %s",
                dt_str(return_type), dt_str(dt)
            );
        }
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
        fprintf(stderr,"Error: not a valid statement\n");
        exit(ERR_INTERNAL);
    }
}

int check_identifier_usage(symbol* id, id_nature nature) {
    if (id->nature == ID_UNDEFINED) {
        semantic_error("identifier '%s' undefined", id->text);
        return 1;
    }

    if (id->nature != nature) {
        semantic_error(
            "'%s' (%s) is not a %s identifier",
            id->text, nature_str(id->nature), nature_str(nature)
        );
        return 2;
    }

    return 0;
}

data_type check_expression(ast* expr) {
    m_location = expr->location;
    int err;

    switch (expr->type) {
    case AST_SYMBOL:
        if (expr->symbol->stype == SYMBOL_IDENTIFIER) {
            check_identifier_usage(expr->symbol, ID_SCALAR);
        }
        return expr->symbol->dtype;
    case AST_VECELEM:
        check_identifier_usage(expr->symbol, ID_VECTOR);
        if (check_expression(expr->children[0]) != DT_INT) {
            semantic_error("vector index must be an integer");
        }
        return expr->symbol->dtype;
    // Unary operations
    case AST_PAREXPR:
    case AST_NOT:
        return check_expression(expr->children[0]);

    // Binary operations
    case AST_SUM:
    case AST_SUB:
    case AST_MUL:
    case AST_DIV:
    case AST_LT:
    case AST_GT:
    case AST_GE:
    case AST_LE:
    case AST_EQ:
    case AST_DIF:
    case AST_AND:
    case AST_OR:
        data_type dt1, dt2;
        dt1 = check_expression(expr->children[0]);
        dt2 = check_expression(expr->children[1]);
        err = check_type_compatibility(dt1, dt2);
        if (err) {
            semantic_error(
                "incompatible types %s and %s in binary operation",
                dt_str(dt1), dt_str(dt2)
            );
            return DT_UNDEFINED;
        }
        return dt1;

    case AST_FUNCCALL:
        if (check_identifier_usage(expr->symbol, ID_FUNC) > 0) {
            return DT_UNDEFINED;
        }
        check_arguments(expr->symbol, expr->children[0]);
        return expr->symbol->dtype;
    case AST_INPUT:
        return n2dtype(expr->children[0]);
    default:
        fprintf(stderr,"Error: not a valid expression\n");
        exit(ERR_INTERNAL);
    }
}

ast* fetch_function_declaration(symbol* func_id) {
    ast* decl_list = m_declarations;

    while (decl_list) {
        if (decl_list->children[0]->symbol == func_id) {
            return decl_list->children[0];
        }
        decl_list = decl_list->children[1];
    }

    fprintf(stderr,"Error: can't find function declaration\n");
    exit(ERR_INTERNAL);
}

void check_arguments(symbol* func_id, ast* arg_list) {
    ast* param_list = fetch_function_declaration(func_id)->children[1];

    m_location = arg_list->location;

    int np = 0; // number of parameters
    int na = 0; // number of arguments
    int err;
    data_type param_dt, arg_dt;
    while (param_list && arg_list) {
        param_dt = param_list->symbol->dtype;
        arg_dt = check_expression(arg_list->children[0]);
        err = check_type_compatibility(param_dt, arg_dt);
        if (err) {
            semantic_error(
                "%s parameter '%s' of function '%s' can't receive %s expression",
                dt_str(param_dt), param_list->symbol->text,
                func_id->text, dt_str(arg_dt)
            );
        }

        param_list = param_list->children[1];
        arg_list = arg_list->children[1];
        np++; na++;
    }

    while (param_list) {
        param_list = param_list->children[1];
        np++;
    }

    while (arg_list) {
        arg_list = arg_list->children[1];
        na++;
    }

    if (np > na) {
        semantic_error(
            "not enough arguments to function '%s', expected %d but got %d",
            func_id->text, np, na
        );
    } else if (na > np) {
        semantic_error(
            "too many arguments to function '%s', expected %d but got %d",
            func_id->text, np, na
        );
    }
}