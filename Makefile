all: main

main: src/main.c
	gcc -o build/rcc src/main.c src/lexer.c src/parser.c src/codegen.c

clean:
	rm build/rcc

test:
	./build/rcc return.c