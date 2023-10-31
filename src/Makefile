#
# Etapa 2 - Makefile
# INF-UFRGS - INF01147 Compiladores - 2023/2
# Guilherme Klein Kern
#

PHASE_NUM=2

all: build

build: lexer parser
	@gcc -c main.c
	@gcc -c hash.c
	@gcc -c lex.yy.c
	@gcc -c y.tab.c
	@gcc -o etapa$(PHASE_NUM) main.o hash.o lex.yy.o y.tab.o

deploy:
	@tar cvzf etapa$(PHASE_NUM).tgz .

lexer:
	@flex --header-file=lex.yy.h scanner.l 

parser:
	@yacc -d -v parser.y

compile:
	@./etapa$(PHASE_NUM) $(target)

clean:
	@rm -f *.o etapa* lex.yy.* y.tab.* y.output