# IR Builder Tests

## Files

The `.c` files are inputs for AST-to-LLVM-IR lowering. They cover expressions,
constant branches and loops, common subexpressions, control flow, nested
scopes, and input/output. The `p1`–`p5` files are stage-specific inputs; the
`p2_common_subexpr`, `p3_const_prop`, `p4_const_prop`, and `p5_const_prop`
files are corresponding IR-generation cases.

`main.c` and `runtime.c` are C test runtimes and are not compiler input.

## Run

```sh
./build/compiler.out tests/ir_builder/control_flow.c /tmp/control_flow.s
```
