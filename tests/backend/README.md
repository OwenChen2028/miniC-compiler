# Backend Tests

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

`max_n.c` and `sum_n.c` call `read()` in a loop and require integer input.

## Additional cases

The files in `my_tests` use `func(4)` in the test runtime:

- `cse_pressure.c` prints 5, 6, 7, 8 twice, then returns 4.
- `repeated_operand.c` prints 1, 1, 1 with input 9, then returns 4.
- `spill_stores.c` prints 5, 6, 7, 8, 99, 100, 104, then returns 4.
