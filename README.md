# MiniC Compiler

## Build

Build the compiler:

```
make compiler
```

Remove the build:
```
make clean
```

## Usage

Run the compiler:
```
./compiler.out input.c output.s
```

Assemble the output:
```
clang -m32 output.s main.c
```

`main.c` must define `int main()`, `int read()`, and `print(int)`
