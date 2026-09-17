# Optimizer Tests

## Files

The `.c` files are MiniC source inputs. Each `<name>.ll` file is optimizer
input, and its `<name>.expected.ll` file is the expected output.

- `fold_add`, `fold_sub`, and `fold_mul` cover arithmetic folding.
- `fold_cmp` covers constant propagation into a comparison.
- `common_subexpression` covers common-subexpression elimination.
- `constant_branch`, `constant_loop`, `constant_loop_variant`, `global`, and
  `local` cover constant propagation.
- `unchanged` covers input that should not change.

`runner.cpp` loads an input module, runs the project optimizer, verifies the
result, and writes the optimized module. `main.c` is a C test runtime. Neither
file is MiniC compiler input.

## Coverage

The test files exercise local constant folding, common-subexpression
elimination, and global constant propagation.

## Run

```sh
./scripts/test.sh
```

The script compares every optimized module with its expected result using
`llvm-diff`.
