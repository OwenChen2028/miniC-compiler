# Tests

## Organization

- `parser`: syntax accepted or rejected by the parser
- `analysis`: programs accepted or rejected by semantic analysis
- `ir_builder`: source programs used to inspect generated LLVM IR
- `optimizer`: optimizer source cases, IR inputs, and expected IR outputs
- `backend`: complete programs compiled to x86 assembly

Each directory contains a README with the files and commands specific to that
test group.

## Backend runtime

`tests/backend/runtime.c` provides `main`, `read`, and `print`. Its optional
command-line argument is passed to `func` and defaults to 4.

## Example

```sh
./build/compiler.out tests/backend/fact.c fact.s
clang -m32 fact.s tests/backend/runtime.c -o fact
./fact 7
```
