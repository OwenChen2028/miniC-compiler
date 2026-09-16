# Parser Tests

## Files

The `.c` files contain accepted and rejected MiniC syntax. `invalid.c` and
`p_bad.c` are rejected inputs. The remaining source files are accepted inputs.

`main.c` is a C test runtime and is not compiler input.

## Run

```sh
./build/compiler.out tests/parser/p1.c /tmp/p1.s
./build/compiler.out tests/parser/p_bad.c /tmp/p_bad.s
```
