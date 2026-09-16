# Optimizer Tests

## Files

The `.c` files are MiniC source inputs. The `.ll` files are LLVM IR inputs and
expected outputs for the optimizer tests. The `cfold_*` and `fold_*` files
cover arithmetic and comparison folding. The `common_subexpression*` files
cover common-subexpression elimination. The `constant_*`, `global*`,
`local*`, `p2_common_subexpr*`, and `p3_const_prop*` through
`p5_const_prop*` files cover propagation and unchanged cases.

`global_opt*`, `local_opt*`, and `no_opt*` are additional source/IR cases.
`main.c` is a C test runtime and is not compiler input.

Files ending in `.ll` without `_opt` or `.expected.ll` are unoptimized inputs.
Files ending in `_opt.ll` or `.expected.ll` are expected optimized outputs.

## Coverage

The test files exercise local constant folding, common-subexpression
elimination, and global constant propagation.

## Validate IR

```sh
llvm-as tests/optimizer/local.ll -o /tmp/local.bc
```
