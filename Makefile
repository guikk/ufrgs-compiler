#
# Etapa 4 - Makefile
# INF-UFRGS - INF01147 Compiladores - 2023/2
# Guilherme Klein Kern
#

CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
PHASE_NUM=4

all: lexer parser build

build: src/main.o src/symbol_table.o src/ast.o src/lex.yy.o src/y.tab.o
	@$(CC) $(CFLAGS) -o etapa$(PHASE_NUM) $^
	@mkdir -p input output

%.o: %.c
	@$(CC) $(CFLAGS) -c $< -o $@

deploy:
	@tar cvzf etapa$(PHASE_NUM).tgz Makefile include/ src/ 

lexer:
	@flex -o src/lex.yy.c --header-file=include/lex.yy.h src/lexer.l 

parser:
	@yacc -d -v src/parser.y
	@mv y.tab.h include/
	@mv y.tab.c src/

compile:
	@./etapa$(PHASE_NUM) input/$(target) output/$(target)

test:
	@./etapa$(PHASE_NUM) input/$(target) output/$(target) > /dev/null 2>&1
	@./etapa$(PHASE_NUM) output/$(target) output/copy-$(target) > /dev/null 2>&1
	@diff output/$(target) output/copy-$(target) && echo "PASS :D" || echo "FAIL! :/"


clean:
	@rm -f src/*.o etapa* */lex.yy.* */y.tab.* y.output output/*