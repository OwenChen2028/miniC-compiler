# Semantic Analysis Tests

## Files

The accepted inputs cover valid scopes and variable shadowing:

- `control_flow.c`
- `nested_shadowing.c`
- `shadowing.c`

The rejected inputs each cover one semantic error:

| File | Error condition |
| --- | --- |
| `duplicate_parameter.c` | Parameter `i` is also declared as a variable. |
| `out_of_scope.c` | Variable `c` is used outside its declaration's scope. |
| `undeclared_variable.c` | Variable `b` is used without declaration. |
| `use_before_declaration.c` | Variable `a` is used before its nested declaration. |

`main.c` is a C test runtime and is not compiler input.

## Run

```sh
./build/compiler.out tests/analysis/control_flow.c /tmp/control_flow.s
./build/compiler.out tests/analysis/undeclared_variable.c /tmp/undeclared.s
```
