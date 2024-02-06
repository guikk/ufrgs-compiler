#
# Makefile
# INF-UFRGS - INF01147 Compiladores - 2023/2
# Guilherme Klein Kern
#

SHELL=/bin/bash -O globstar

CC = clang
CFLAGS = -Wall -Wextra -Iinclude -g
PHASE_NUM=6

all: lexer parser build

build: src/main.o src/symbol_table.o src/ast.o src/semantics.o src/tac.o src/asm.o src/lex.yy.o src/y.tab.o
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
	@./etapa$(PHASE_NUM) $(target)

run:
	@gcc $(target) -no-pie -o $(target:.s=.out) && ./$(target:.s=.out)

clean:
	@rm -f etapa* */lex.yy.* */y.* y.output output/* **/*.s **/*.o **/*.out