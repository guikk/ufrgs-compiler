#
# Etapa 4 - Makefile
# INF-UFRGS - INF01147 Compiladores - 2023/2
# Guilherme Klein Kern
#

CC = clang
CFLAGS = -Wall -Wextra -Iinclude -g
PHASE_NUM=5

all: lexer parser build

build: src/main.o src/symbol_table.o src/ast.o src/semantics.o src/tac.o src/lex.yy.o src/y.tab.o
	@echo "Building executable 'etapa$(PHASE_NUM)'"
	@$(CC) $(CFLAGS) -o etapa$(PHASE_NUM) $^
	@mkdir -p input output

%.o: %.c
	@$(CC) $(CFLAGS) -c $< -o $@

deploy:
	@tar cvzf etapa$(PHASE_NUM).tgz Makefile include/ src/ 

lexer:
	@echo "Generating lexer from 'lexer.l'"
	@flex -o src/lex.yy.c --header-file=include/lex.yy.h src/lexer.l 

parser:
	@echo "Generating parser from 'parser.y'"
	@yacc -d -v src/parser.y
	@mv y.tab.h include/
	@mv y.tab.c src/

compile:
	@./etapa$(PHASE_NUM) $(target) $(out)

test_decompile:
	@./etapa$(PHASE_NUM) input/$(target) output/$(target) > /dev/null 2>&1
	@./etapa$(PHASE_NUM) output/$(target) output/copy-$(target) > /dev/null 2>&1
	@diff output/$(target) output/copy-$(target) && echo "PASS :D" || echo "FAIL! :/"


clean:
	@rm -f src/*.o etapa* */lex.yy.* */y.tab.* y.output output/*