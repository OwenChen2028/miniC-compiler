# Tests

Tests are grouped by compiler stage:

- `parser`: accepted and rejected syntax
- `analysis`: accepted programs and one semantic error per rejected program
- `ir`: control-flow and expression lowering
- `optimizer`: source and LLVM IR fixtures; `.expected.ll` is expected output
- `backend`: complete programs compiled to x86

`runtime.c` files provide `main`, `read`, and `print` where execution is useful.
The backend runtime also checks stack alignment. Its optional argument is passed
to `func` and defaults to 4.

```sh
./build/compiler.out tests/backend/fact.c fact.s
clang -m32 fact.s tests/backend/runtime.c -o fact
./fact 7
```
