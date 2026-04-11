all: main

main: src/main.c
	gcc -o build/rcc src/main.c

clean:
	rm build/rcc
