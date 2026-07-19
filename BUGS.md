# Bugs

## Spill handling

This is not a functional correctness issue for the current frontend. Integer
expressions contain at most one binary operation, and their results are stored
right away, so the generated IR does not create enough live SSA temporaries to
expose the problem.

Spill handling is incomplete for more general LLVM IR. The register allocator
marks spilled values as `nullreg`, but `getOffsetMap()` only assigns locations
in a few cases:

- allocas get their own stack slots
- loads reuse the slot they load from
- stored values reuse the destination slot

There is no guarantee that every spilled SSA result gets a slot. If one is used
before being stored, code generation can crash in `offset_map.at()`.

Test case: `tests/backend/my_tests/large_spill.ll`

## Fixed

### Division

The parser accepted division and the IR builder emitted `sdiv`, but the backend
did not support it. Division has been removed from the grammar, AST operators,
and IR builder.

### Constant conditions

LLVM can fold a constant comparison into an `i1`. The backend expected an
`icmp` instruction and could branch the wrong way. Constant branch operands are
now handled directly.

### Error status

Parser and analysis errors used to return exit status 0. They now return a
nonzero status and do not create or truncate the output file.
