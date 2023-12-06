#
# Etapa 3 - Makefile
# INF-UFRGS - INF01147 Compiladores - 2023/2
# Guilherme Klein Kern
#

PHASE_NUM=3

all: build

build: lexer parser
	@gcc -c main.c
	@gcc -c symbol_table.c
	@gcc -c ast.c
	@gcc -c lex.yy.c
	@gcc -c y.tab.c
	@gcc -o etapa$(PHASE_NUM) \
	main.o symbol_table.o ast.o lex.yy.o y.tab.o
	@mkdir -p input output

deploy:
	@tar cvzf etapa$(PHASE_NUM).tgz .

lexer:
	@flex --header-file=lex.yy.h lexer.l 

parser:
	@yacc -d -v parser.y

compile:
	@./etapa$(PHASE_NUM) input/$(target) output/$(target)

test:
	@./etapa$(PHASE_NUM) input/$(target) output/$(target) > /dev/null 2>&1
	@./etapa$(PHASE_NUM) output/$(target) output/copy-$(target) > /dev/null 2>&1
	@diff output/$(target) output/copy-$(target) && echo "PASS :D" || echo "FAIL! :/"


clean:
	@rm -f *.o etapa* lex.yy.* y.tab.* y.output output/*