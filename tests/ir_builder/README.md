# IR Builder Tests

## Files

The `.c` files are inputs for AST-to-LLVM-IR lowering. They cover expressions,
constant branches and loops, common subexpressions, control flow, nested
scopes, and input/output.

`runtime.c` is a C test runtime and is not compiler input.

## Run

```sh
./build/compiler.out tests/ir_builder/control_flow.c /tmp/control_flow.s
```
