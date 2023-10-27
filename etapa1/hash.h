/* 
	ETAPA 1 - Compiladores - 2019/1 Turma A - Prof. Marcelo Johann
	Alunos:
		Guilherme Haetinger e Lucas Alegre 
*/

#ifndef HASH
#define HASH

#define HASH_SIZE 997

typedef struct hash_node{
    int type;
    char* text;
    struct hash_node* next;
} hash_node;

int hash(char *text);

hash_node* add_symbol(int type, char* text);

hash_node* get_symbol(char *text);

void init_symbol_table(void);

void print_symbol_table(void);

#endif
