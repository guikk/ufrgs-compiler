all: build

build:
	gcc -c lex.yy.c
	gcc -c main.c
	gcc -c hash.c
	gcc -o ufrgs-compiler lex.yy.o main.o hash.o

compile:
	./ufrgs-compiler $(filename)

clean: rm -rf *.o