# rcc

A minimal C compiler written in C, targeting x64 assembly. This project follows the structure of Nora Sandler's "Writing a C Compiler" book.

Currently, the compiler has been implemented up to Chapter 2 (Unary Operators). 

## Features
* Basic function structure (`int main(void)`)
* Integer return values
* Unary operators: `-` (negation) and `~` (bitwise complement)
* Nested expressions (e.g., `return ~(-(~2));`)
* Automatic assembly and linking via `gcc`

## Pipeline
The compiler translates C code into an executable through four distinct stages:

1. **Lexical Analysis (`lexer.c`)**: Reads the source file and converts it into a stream of tokens.
2. **Parsing (`parser.c`)**: A recursive descent parser that validates the grammar and builds an Abstract Syntax Tree (AST).
3. **TACKY IR Generation (`tacker.c`)**: Flattens the nested AST into Three-Address Code (TACKY), a linear intermediate representation that introduces temporary variables for complex expressions.
4. **Code Generation (`codegen.c`)**: 
   * Maps TACKY instructions to an Assembly AST.
   * Allocates stack memory for temporary variables.
   * Fixes illegal x64 instructions (e.g., routing memory-to-memory moves through the `%r10d` scratch register).
   * Emits the final AT&T syntax assembly to `assembly.s`.

## Usage

Build the compiler:
```bash
make
```
Run the compiler on a C file:

```bash
./build/rcc return.c
```

The compiler will generate an assembly.s file and automatically call gcc to build the final executable (out_program).

To test the compiled program's output (which returns via the exit code):

```bash
./out_program
echo $?
```

(Note: Linux exit codes wrap at 256, so return -4 will print as 252)