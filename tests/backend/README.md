# Backend Tests

## Files

The `.c` files are complete MiniC programs compiled to x86 assembly. `fact.c`,
`fib.c`, `rem_2.c`, and `square.c` cover arithmetic and function execution.
`max_n.c` and `sum_n.c` call `read()` in a loop and require integer input.
`cse_pressure.c`, `repeated_operand.c`, `spill_stores.c`, and `stress.c` cover
register-allocation cases.

`main.c` and `runtime.c` are C test runtimes and are not compiler input.

## Run

Generate assembly for a source file, link it with the test runtime, and run the
result:

```sh
./build/compiler.out tests/backend/fact.c fact.s
clang -m32 fact.s tests/backend/runtime.c -o fact
./fact 7
```

Run the same commands for the other source files in `tests/backend`, excluding
`main.c` and `runtime.c`.
