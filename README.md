# MiniC Compiler

This compiler translates a subset of C into 32-bit x86 assembly. It is written
in C++ and builds its intermediate representation with the LLVM 18 C API.

## Implementation

| Stage | Implementation | Source |
| --- | --- | --- |
| Lexer and parser | Builds an AST with Flex and Bison | [`src/lexer.l`](src/lexer.l), [`src/parser.y`](src/parser.y), [`src/ast.cpp`](src/ast.cpp) |
| Semantic analysis | Rejects undeclared variables and duplicate declarations in the same scope | [`src/analysis.cpp`](src/analysis.cpp) |
| IR generation | Lowers the AST to LLVM IR and constructs basic blocks | [`src/ir_builder.cpp`](src/ir_builder.cpp) |
| Optimization | Folds and propagates constants and eliminates common subexpressions and dead code | [`src/optimizer.cpp`](src/optimizer.cpp) |
| x86 backend | Lowers LLVM IR to x86 assembly with live-range-based register allocation | [`src/backend.cpp`](src/backend.cpp) |

The backend allocates values to `ebx`, `ecx`, and `edx`. Function results use
`eax`, and local variables are stored in a stack frame.

## Language subset

- an integer-returning function with an optional integer parameter
- integer variables, nested scopes, and variable shadowing
- assignments and integer constants
- `+`, `-`, `*`, and unary negation
- `==`, `!=`, `<`, `<=`, `>`, and `>=`
- `if`/`else`, `while`, and `return`
- input and output through `read()` and `print(int)`

Example input:

```c
extern void print(int);
extern int read();

int func(int n) {
    int product;
    int i;
    i = 1;
    product = 1;

    while (i < n) {
        i = i + 1;
        product = product * i;
    }

    return product;
}
```

## Build and run

Requirements:

- C++17 compiler
- CMake 3.20 or newer
- LLVM 18 development libraries
- Flex and Bison (`lex` and `yacc`)
- Clang and 32-bit development libraries

On Ubuntu, these are provided by `g++`, `cmake`, `llvm-18-dev`, `flex`, `bison`,
`clang`, and `libc6-dev-i386`.

Build:

```sh
cmake -S . -B build
cmake --build build
```

Compile and run the factorial example:

```sh
./build/compiler.out tests/backend/fact.c factorial.s
clang -m32 factorial.s tests/backend/main.c -o factorial
./factorial
```

Expected output:

```text
In main printing return value of test: 24
```

Usage:

```text
./build/compiler.out <input.c> [output.s]
```

If `output.s` is omitted, assembly is written to standard output. The linked
runtime must define `int main()`, `int read()`, and `void print(int)`; see
[`tests/backend/main.c`](tests/backend/main.c) for a minimal implementation.

Clean:

```sh
cmake --build build --target clean
```

## Tests

The [`tests`](tests) directory is organized by compiler stage:

- `tests/parser`: parser inputs
- `tests/analysis`: semantic-analysis inputs
- `tests/ir_builder`: expression and control-flow inputs
- `tests/optimizer`: optimizer-pass IR test files and source examples
- `tests/backend`: programs for checking generated assembly

The backend tests include factorial, Fibonacci, summation, remainder, and
input-driven loops. Use the compile, link, and run commands above with any of
the source files in `tests/backend`.
