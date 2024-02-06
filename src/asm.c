/*
 * asm.c
 * INF-UFRGS - INF01147 Compiladores - 2023/2
 * Guilherme Klein Kern
 */

#include "asm.h"
#include "errors.h"
#include "symbol_table.h"
#include "semantics.h"

#include <stdio.h>
#include <stdlib.h>

int m_branch_label = 0;
int m_str_const = 0;

void asm_gen_data(FILE* out);
void asm_gen_var(FILE* out, symbol* s, char* value);
void asm_gen_vardef(FILE* out, symbol* s, int size);
void asm_gen_init(FILE* out, data_type dt, char* value);
void asm_gen_decl(FILE* out, ast* decl);
void asm_gen_tac(FILE* out, tac* t);
void asm_gen_call(FILE* out, tac* t);

void generate_asm(ast* decl_list, tac* tac_list, char *output_file) {
    FILE *out = fopen(output_file, "w");
    if(out == NULL) {
        fprintf(stderr, "Error creating assembly file %s\n", output_file);
        exit(ERR_OPEN_FILE);
    }

    asm_gen_data(out);

    // fprintf(out, ".section .text\n");
    // fprintf(out, "    .global _start\n");
    // fprintf(out, "_start:\n");

    for(ast* dl = decl_list; dl != NULL; dl = dl->children[1]) {
        asm_gen_decl(out, dl->children[0]);
    }

    for(tac* t = tac_list; t != NULL; t = t->next) {
        asm_gen_tac(out, t);
    }
    
    fclose(out);
    fprintf(stderr, "\033[0;32massembly file %s created successfully :)\n", output_file);
}

void asm_gen_data(FILE* out) {
    symbol** table = get_symbol_table();
    int i;
    symbol* s;

    fprintf(out, ".section .data\n");
    fprintf(out, "    .fmt_int: .string \"%%d\\n\"\n");
    fprintf(out, "    .fmt_float: .string \"%%f\\n\"\n");
    fprintf(out, "    .fmt_char: .string \"%%c\\n\"\n");
    fprintf(out, "    .scan_int: .string \"%%d\"\n");
    fprintf(out, "    .scan_float: .string \"%%f\"\n");
    fprintf(out, "    .scan_char: .string \"%%c\"\n");
    fprintf(out, "    .CRLF: .string \"\\n\"\n");

    for (i = 0; i < HASH_SIZE; i++) {
        for (s = table[i]; s != NULL; s = s->next) {
            switch(s->stype) {
            case SYMBOL_TEMP:
                asm_gen_var(out, s, NULL); break;
            case SYMBOL_LIT_INT:
            case SYMBOL_LIT_FLOAT:
            case SYMBOL_LIT_CHAR:
                asm_gen_var(out, s, s->text); break;
            case SYMBOL_LIT_STRING:
                fprintf(out, ".str_%d: .string %s\n", m_str_const, s->text);
                s->str_index = m_str_const;
                m_str_const++;
                break;
            default:
                break;
            }
        }
    }
}

void asm_gen_var(FILE* out, symbol* s, char* value) {
        fprintf(out, "_%s:\n", s->text);
        asm_gen_init(out, s->dtype, value);
        asm_gen_vardef(out, s, 4);
}

void asm_gen_vardef(FILE* out, symbol* s, int size) {
    fprintf(out, "    .globl _%s\n", s->text);
    fprintf(out, "    .data\n");
    fprintf(out, "    .type _%s, @object\n", s->text);
    fprintf(out, "    .size _%s, %d\n", s->text, size);
}

void asm_gen_init(FILE* out, data_type dt, char* value) {
    if (dt == DT_FLOAT && value != NULL) {
        fprintf(out, "    .float %s\n", value);
        return;
    }
    if (dt == DT_FLOAT && value == NULL) {
        fprintf(out, "    .float 0.0\n");
        return;
    }
    if (value != NULL) {
        fprintf(out, "    .long %s\n", value);
        return;
    }
    fprintf(out, "    .long 0\n");
}

void asm_gen_decl(FILE* out, ast* decl) {
    symbol* s = decl->symbol;
    ast* v;
    if (decl->type == AST_DECL_VAR) {
        asm_gen_var(out, s, decl->children[1]->symbol->text);
        return;
    }
    if (decl->type == AST_DECL_VEC) {
        fprintf(out, "_%s:\n", s->text);
        for(v = decl->children[2]; v != NULL; v = v->children[1]) {
            asm_gen_init(out, s->dtype, v->children[0]->symbol->text);
        }
        asm_gen_init(out, s->dtype, 0);
        asm_gen_vardef(out, s, 4);
        return;
    }
    if (decl->type == AST_DECL_FUNC) {
        for (v = decl->children[1]; v != NULL; v = v->children[1]) {
            asm_gen_var(out, v->symbol, NULL);
        }
        return;
    }
}

void asm_gen_tac(FILE* out, tac* t) {
    if (t->op == TAC_NOP || t->op == TAC_SYMBOL) {
        return;
    }

    fprintf(out, "# ");
    tac_fprint(out, t);

    switch(t->op) {
    case TAC_MOVE:
        fprintf(out, "    movl _%s, %%eax\n", t->arg1->text);
        fprintf(out, "    movl %%eax, _%s\n", t->result->text);
        break;
    case TAC_ADD:
        fprintf(out, "    movl _%s, %%eax\n", t->arg1->text);
        fprintf(out, "    addl _%s, %%eax\n", t->arg2->text);
        fprintf(out, "    movl %%eax, _%s\n", t->result->text);
        break;
    case TAC_SUB:
        fprintf(out, "    movl _%s, %%eax\n", t->arg1->text);
        fprintf(out, "    subl _%s, %%eax\n", t->arg2->text);
        fprintf(out, "    movl %%eax, _%s\n", t->result->text);
        break;
    case TAC_MUL:
        fprintf(out, "    movl _%s, %%eax\n", t->arg1->text);
        fprintf(out, "    imull _%s, %%eax\n", t->arg2->text);
        fprintf(out, "    movl %%eax, _%s\n", t->result->text);
        break;
    case TAC_DIV:
        fprintf(out, "    movl _%s, %%eax\n", t->arg1->text);
        fprintf(out, "    cltd\n");
        fprintf(out, "    idivl _%s\n", t->arg2->text);
        fprintf(out, "    movl %%eax, _%s\n", t->result->text);
        break;
    case TAC_LT:
        fprintf(out, "    movl _%s, %%edx\n", t->arg1-> text);
        fprintf(out, "    movl _%s, %%eax\n", t->arg2-> text);
        fprintf(out, "    cmpl %%eax, %%edx\n");
        fprintf(out, "    jl .BL%d\n", m_branch_label);
        fprintf(out, "    movl $0, %%eax\n");
        fprintf(out, "    jmp .BL%d\n", m_branch_label+1);
        fprintf(out, ".BL%d:\n", m_branch_label);
        fprintf(out, "    movl $1, %%eax\n");
        fprintf(out, ".BL%d:\n", m_branch_label+1);
        fprintf(out, "    movl %%eax, _%s\n", t->result->text);
        m_branch_label += 2;
        break;
    case TAC_GT:
        fprintf(out, "    movl _%s, %%edx\n", t->arg1-> text);
        fprintf(out, "    movl _%s, %%eax\n", t->arg2-> text);
        fprintf(out, "    cmpl %%eax, %%edx\n");
        fprintf(out, "    jg .BL%d\n", m_branch_label);
        fprintf(out, "    movl $0, %%eax\n");
        fprintf(out, "    jmp .BL%d\n", m_branch_label+1);
        fprintf(out, ".BL%d:\n", m_branch_label);
        fprintf(out, "    movl $1, %%eax\n");
        fprintf(out, ".BL%d:\n", m_branch_label+1);
        fprintf(out, "    movl %%eax, _%s\n", t->result->text);
        m_branch_label += 2;
        break;
    case TAC_GE:
        fprintf(out, "    movl _%s, %%edx\n", t->arg1-> text);
        fprintf(out, "    movl _%s, %%eax\n", t->arg2-> text);
        fprintf(out, "    cmpl %%eax, %%edx\n");
        fprintf(out, "    jge .BL%d\n", m_branch_label);
        fprintf(out, "    movl $0, %%eax\n");
        fprintf(out, "    jmp .BL%d\n", m_branch_label+1);
        fprintf(out, ".BL%d:\n", m_branch_label);
        fprintf(out, "    movl $1, %%eax\n");
        fprintf(out, ".BL%d:\n", m_branch_label+1);
        fprintf(out, "    movl %%eax, _%s\n", t->result->text);
        m_branch_label += 2;
        break;
    case TAC_LE:
        fprintf(out, "    movl _%s, %%edx\n", t->arg1-> text);
        fprintf(out, "    movl _%s, %%eax\n", t->arg2-> text);
        fprintf(out, "    cmpl %%eax, %%edx\n");
        fprintf(out, "    jle .BL%d\n", m_branch_label);
        fprintf(out, "    movl $0, %%eax\n");
        fprintf(out, "    jmp .BL%d\n", m_branch_label+1);
        fprintf(out, ".BL%d:\n", m_branch_label);
        fprintf(out, "    movl $1, %%eax\n");
        fprintf(out, ".BL%d:\n", m_branch_label+1);
        fprintf(out, "    movl %%eax, _%s\n", t->result->text);
        m_branch_label += 2;
        break;
    case TAC_EQ:
        fprintf(out, "    movl _%s, %%edx\n", t->arg1-> text);
        fprintf(out, "    movl _%s, %%eax\n", t->arg2-> text);
        fprintf(out, "    cmpl %%eax, %%edx\n");
        fprintf(out, "    je .BL%d\n", m_branch_label);
        fprintf(out, "    movl $0, %%eax\n");
        fprintf(out, "    jmp .BL%d\n", m_branch_label+1);
        fprintf(out, ".BL%d:\n", m_branch_label);
        fprintf(out, "    movl $1, %%eax\n");
        fprintf(out, ".BL%d:\n", m_branch_label+1);
        fprintf(out, "    movl %%eax, _%s\n", t->result->text);
        m_branch_label += 2;
        break;
    case TAC_DIF:
        fprintf(out, "    movl _%s, %%edx\n", t->arg1-> text);
        fprintf(out, "    movl _%s, %%eax\n", t->arg2-> text);
        fprintf(out, "    cmpl %%eax, %%edx\n");
        fprintf(out, "    jne .BL%d\n", m_branch_label);
        fprintf(out, "    movl $0, %%eax\n");
        fprintf(out, "    jmp .BL%d\n", m_branch_label+1);
        fprintf(out, ".BL%d:\n", m_branch_label);
        fprintf(out, "    movl $1, %%eax\n");
        fprintf(out, ".BL%d:\n", m_branch_label+1);
        fprintf(out, "    movl %%eax, _%s\n", t->result->text);
        m_branch_label += 2;
        break;
    case TAC_AND:
        fprintf(out, "    movl _%s, %%edx\n", t->arg1-> text);
        fprintf(out, "    movl _%s, %%eax\n", t->arg2-> text);
        fprintf(out, "    andl %%eax, %%edx\n");
        fprintf(out, "    jz .BL%d\n", m_branch_label);
        fprintf(out, "    movl $1, %%eax\n");
        fprintf(out, "    jmp .BL%d\n", m_branch_label+1);
        fprintf(out, ".BL%d:\n", m_branch_label);
        fprintf(out, "    movl $0, %%eax\n");
        fprintf(out, ".BL%d:\n", m_branch_label+1);
        fprintf(out, "    movl %%eax, _%s\n", t->result->text);
        m_branch_label += 2;
        break;
    case TAC_OR:
        fprintf(out, "    movl _%s, %%edx\n", t->arg1-> text);
        fprintf(out, "    movl _%s, %%eax\n", t->arg2-> text);
        fprintf(out, "    orl %%eax, %%edx\n");
        fprintf(out, "    jz .BL%d\n", m_branch_label);
        fprintf(out, "    movl $1, %%eax\n");
        fprintf(out, "    jmp .BL%d\n", m_branch_label+1);
        fprintf(out, ".BL%d:\n", m_branch_label);
        fprintf(out, "    movl $0, %%eax\n");
        fprintf(out, ".BL%d:\n", m_branch_label+1);
        fprintf(out, "    movl %%eax, _%s\n", t->result->text);
        m_branch_label += 2;
        break;
    case TAC_NOT:
        fprintf(out, "    movl _%s, %%edx\n", t->arg1-> text);
        fprintf(out, "    movl $1, %%eax\n");
        fprintf(out, "    xorl %%eax, %%edx\n");
        fprintf(out, "    movl %%edx, _%s\n", t->result->text);
        break;
    case TAC_LABEL:
        fprintf(out, ".%s:\n", t->arg1->text);
        break;
    case TAC_BEGINFUN:
        fprintf(out, ".text\n");
        fprintf(out, ".globl %s\n", t->arg1->text);
        fprintf(out, ".type %s, @function\n", t->arg1->text);
        fprintf(out, "%s:\n", t->arg1->text);
        fprintf(out, "    pushq %%rbp\n");
        fprintf(out, "    movq %%rsp, %%rbp\n");
        break;
    case TAC_ENDFUN:
        fprintf(out, "    popq %%rbp\n");
        fprintf(out, "    ret\n");
        break;
    case TAC_IFZ:
        fprintf(out, "    movl _%s, %%eax\n", t->arg1->text);
        fprintf(out, "    andl $1, %%eax\n");
        fprintf(out, "    jz .%s\n", t->result->text);
        break;
    case TAC_JUMP:
        fprintf(out, "    jmp .%s\n", t->result->text);
        break;
    case TAC_CALL:
        asm_gen_call(out, t);
        break;
    case TAC_ARG:
        fprintf(out, "    movl _%s, %%eax\n", t->arg1->text);
        fprintf(out, "    pushq %%rax\n");
        break;
    case TAC_RET:
        fprintf(out, "    movl _%s, %%eax\n", t->arg1->text);
        fprintf(out, "    popq %%rbp\n");
        fprintf(out, "    ret\n");
        break;
    case TAC_PRINT:
        if (t->arg1->stype == SYMBOL_LIT_STRING) {
            fprintf(out, "    leaq .str_%d, %%rdi\n", t->arg1->str_index);
            fprintf(out, "    movl $0, %%eax\n");
            fprintf(out, "    call printf@PLT\n");
            fprintf(out, "    leaq .CRLF, %%rdi\n");
        } else {
            fprintf(out, "    movl _%s, %%eax\n", t->arg1->text);
            fprintf(out, "    movl %%eax, %%esi\n");
            fprintf(out, "    leaq .fmt_%s, %%rdi\n", dt_str(t->arg1->dtype));
            fprintf(out, "    movl $0, %%eax\n");
            fprintf(out, "    call printf@PLT\n");
        }
        break;
    case TAC_INPUT:
        fprintf(out, "    movl $_%s, %%esi\n", t->result->text);
        fprintf(out, "    leaq .scan_%s, %%rdi\n", dt_str(t->result->dtype));
        fprintf(out, "    call __isoc99_scanf@PLT\n");
        break;
    // case TAC_VECSET:
    // case TAC_VECGET:
    
    default:
        break;
    }
}

void asm_gen_call(FILE* out, tac* t) {
    ast* decl = fetch_function_declaration(t->arg1);
    ast* param;

    for (param = decl->children[1]; param != NULL; param = param->children[1]) {
        fprintf(out, "    popq %%rax\n");
        fprintf(out, "    movl %%eax, _%s\n", param->symbol->text);
    }

    fprintf(out, "    call %s\n", t->arg1->text);
    fprintf(out, "    movl %%eax, _%s\n", t->result->text);
}