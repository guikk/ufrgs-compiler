#
# Etapa 1 - Makefile
# INF-UFRGS - INF01147 Compiladores - 2023/2
# Guilherme Klein Kern
#

PHASE_NUM=1

all: build

build: lexer
	@gcc -c lex.yy.c
	@gcc -c main.c
	@gcc -c hash.c
	@gcc -o etapa$(PHASE_NUM) lex.yy.o main.o hash.o

deploy:
	@tar cvzf etapa$(PHASE_NUM).tgz .

lexer:
	@flex --header-file=lex.yy.h scanner.l 

compile:
	@./etapa$(PHASE_NUM) $(target)

clean:
	@rm -f *.o lex.yy.* etapa*