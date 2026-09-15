# Bugs

## Backend IR limitations

The backend supports the IR produced by the MiniC frontend, not arbitrary LLVM
IR. Register allocation is local to each basic block: SSA temporaries cannot
carry values between blocks, and `phi` instructions are not supported.
Comparisons set flags for an immediately following conditional branch; they do
not produce boolean values that can be stored or used in arithmetic.

## Fixed: spill storage and register reuse

Spilled values now have dedicated stack slots, and spilled loads save their
value when the load executes. Previously, missing slots could crash code
generation, and sharing a variable's slot could change an already computed
value after a store. Common-subexpression elimination could expose these
problems even with the current frontend's simple expressions.

Reusing a dying operand's register also keeps it reserved for the result when
both operands are the same value, as in `n + n`.

Regression tests are in `tests/backend/my_tests`; see `tests/backend/README`
for commands and expected results.
