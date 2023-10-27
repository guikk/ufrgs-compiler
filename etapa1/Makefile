#
# Etapa 1 - Makefile
# INF-UFRGS - INF01147 Compiladores - 2023/2
# Guilherme Klein Kern
#

all: build

build: lexer
	@gcc -c lex.yy.c
	@gcc -c main.c
	@gcc -c hash.c
	@gcc -o etapa1 lex.yy.o main.o hash.o

lexer:
	@flex --header-file=lex.yy.h scanner.l 

compile:
	@./etapa1 $(target)

clean:
	@rm -f *.o lex.yy.* etapa1