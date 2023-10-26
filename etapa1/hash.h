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

hash_node* Table[HASH_SIZE];


void hashInit(void);

hash_node* hashInsert(int type, char* text);

hash_node* hashFind(char *text);

int hashAddress(char *text);

void hashPrint(void);

#endif
