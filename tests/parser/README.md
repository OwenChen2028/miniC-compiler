# Parser Tests

## Files

The `.c` files are parser inputs. `invalid.c` and `p_bad.c` contain rejected
syntax. `p1.c` through `p5.c`, `control_flow.c`, `io_loop.c`,
`nested_loops.c`, `nested_read.c`, and `shadowing.c` contain accepted syntax.

`main.c` is a C test runtime and is not compiler input.

## Run

```sh
./build/compiler.out tests/parser/p1.c /tmp/p1.s
./build/compiler.out tests/parser/p_bad.c /tmp/p_bad.s
```

The command should succeed for an accepted input and report an error for a
rejected input.
