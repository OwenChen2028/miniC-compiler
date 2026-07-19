# Bugs and Known Issues

## Fixed

### Constant relational conditions used the wrong branch

Programs such as:

```c
extern void print(int);
extern int read();
int func() {
  if (1 < 2)
    return 11;
  else
    return 22;
}
```

compiled to an unconditional jump to the false branch.

Cause: `LLVMBuildICmp` can fold comparisons with constant operands into a
constant `i1`. The backend assumed every conditional branch operand was an
`icmp` instruction and called `LLVMGetICmpPredicate` on it. For a constant
condition, no predicate matched, so code generation fell through to the false
successor jump.

Status: fixed in `src/backend.c` by detecting constant integer branch
conditions and emitting a direct jump to the selected successor.

### Invalid input exits with status code 0

Parser and semantic failures previously printed an error message but returned
process status 0.

Examples:

```sh
./compiler.out tests/frontend/parser/p_bad.c out.s
./compiler.out tests/frontend/analysis/p1_bad.c out.s
```

Status: fixed in `src/compiler.c` by propagating parser and semantic analysis
failures to the process exit status. Invalid input can still leave an empty
output file behind.

## Latent Backend Issue

### Register spills need stack slots for arbitrary SSA temporaries

The current spill handling is not general enough for arbitrary LLVM IR.
`getOffsetMap()` creates offsets for allocas, aliases loads to their source
slots, and gives non-constant store sources the destination slot. It does not
allocate independent stack slots for all spilled instruction results.

If an SSA temporary is spilled and then used before it has ever been stored,
code generation can fail at `offset_map.at(...)`.

Confirmed with the backend-only fixture:

```sh
tests/backend/my_tests/large_spill.ll
```

That IR creates many live intermediate SSA values before the final store. It can
make the backend crash with `unordered_map::at` when a spilled temporary has no
offset.

This does not appear to be reachable from normal miniC source today because the
grammar only allows one binary operator per integer expression and the IR
builder stores assignment/return results quickly. It is still a real backend
limitation if handwritten or more general LLVM IR is supported.
