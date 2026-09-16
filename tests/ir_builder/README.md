# IR Builder Tests

## Files

The `.c` files exercise AST-to-LLVM-IR lowering for expressions, control flow,
nested scopes, and input/output. The `p1`–`p5` files are stage-specific inputs.

`main.c` and `runtime.c` are C test runtimes and are not compiler input.

## Run

```sh
./build/compiler.out tests/ir_builder/control_flow.c /tmp/control_flow.s
```
