# Parser Tests

## Files

The `.c` files are parser inputs. `invalid.c` contains rejected syntax.
`control_flow.c`, `io_loop.c`, `nested_loops.c`, `nested_read.c`, and
`shadowing.c` contain accepted syntax.

`main.c` is a C test runtime and is not compiler input.

## Run

```sh
./build/compiler.out tests/parser/control_flow.c /tmp/control_flow.s
./build/compiler.out tests/parser/invalid.c /tmp/invalid.s
```

The command should succeed for an accepted input and report an error for a
rejected input.
