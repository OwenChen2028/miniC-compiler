# Additional Backend Tests

## Files

These source files exercise repeated expressions, register pressure, and
spilled values across calls and stores.

## Run

```sh
./build/compiler.out tests/backend/my_tests/cse_pressure.c /tmp/cse_pressure.s
clang -m32 /tmp/cse_pressure.s tests/backend/runtime.c -o /tmp/cse_pressure
/tmp/cse_pressure 4
```
