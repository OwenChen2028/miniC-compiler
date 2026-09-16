# Tests

## Organization

- `parser`: accepted and rejected syntax
- `analysis`: accepted programs and one semantic error per rejected program
- `ir_builder`: control-flow and expression lowering
- `optimizer`: source examples and standalone LLVM IR pass test files
- `backend`: complete programs compiled to x86

## Backend test runtime

`tests/backend/runtime.c` provides `main`, `read`, and `print`. It also checks
stack alignment. Its optional command-line argument is passed to `func` and
defaults to 4.

## Example

```sh
./build/compiler.out tests/backend/fact.c fact.s
clang -m32 fact.s tests/backend/runtime.c -o fact
./fact 7
```
