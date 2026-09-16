# Optimizer Tests

## Files

The `.c` files are MiniC source inputs. The `.ll` files are LLVM IR inputs and
expected outputs for the optimizer tests.

Files ending in `.ll` without `_opt` or `.expected.ll` are unoptimized inputs.
Files ending in `_opt.ll` or `.expected.ll` are expected optimized outputs.

## Coverage

The test files exercise local constant folding, common-subexpression
elimination, and global constant propagation.

## Validate IR

```sh
llvm-as tests/optimizer/local.ll -o /tmp/local.bc
```
