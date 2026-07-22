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
./compiler.out input.cpp output.s
```

Assemble the output:
```
clang++ -m32 output.s main.cpp
```

`main.cpp` must define `int main()`, `int read()`, and `print(int)`
