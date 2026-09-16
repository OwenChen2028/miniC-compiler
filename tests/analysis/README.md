# Semantic Analysis Tests

## Files

Files with the suffix `good` should pass semantic analysis. Files with the
suffix `bad` should be rejected for these conditions:

| File | Error condition |
| --- | --- |
| `p1_bad.c` | Parameter `i` is also declared as a variable. |
| `p2_bad.c` | Variable `b` is used without declaration. |
| `p3_bad.c` | Variable `c` is used outside the declaration's scope in the return statement. |
| `p4_bad.c` | Variable `a` is used before its declaration in a nested scope. |

`main.c` is a C test runtime and is not compiler input.

## Run

```sh
./build/compiler.out tests/analysis/p1_good.c /tmp/p1_good.s
./build/compiler.out tests/analysis/p1_bad.c /tmp/p1_bad.s
```
