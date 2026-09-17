#!/usr/bin/env bash

set -euo pipefail

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
BUILD_DIR=${BUILD_DIR:-"$ROOT/build"}
COMPILER="$BUILD_DIR/compiler.out"
OPTIMIZER_RUNNER="$BUILD_DIR/optimizer_runner"
CC=${CC:-clang}
LLVM_DIFF=${LLVM_DIFF:-llvm-diff}
RUNTIME="$ROOT/tests/backend/runtime.c"
TMP_DIR=$(mktemp -d)

trap 'rm -rf "$TMP_DIR"' EXIT

cmake -S "$ROOT" -B "$BUILD_DIR"
cmake --build "$BUILD_DIR"

assembly_for() {
  printf '%s/%s.s\n' "$TMP_DIR" "$(basename "$1" .c)"
}

compile_source() {
  local source=$1
  "$COMPILER" "$source" "$(assembly_for "$source")"
}

expect_compile() {
  local source=$1
  compile_source "$source"
  echo "PASS $source"
}

expect_reject() {
  local source=$1

  if compile_source "$source" >/dev/null 2>&1; then
    echo "FAIL $source: expected the compiler to reject it" >&2
    return 1
  fi
  echo "PASS $source (rejected)"
}

for source in "$ROOT"/tests/parser/*.c; do
  case "$(basename "$source")" in
    main.c) ;;
    invalid.c) expect_reject "$source" ;;
    *) expect_compile "$source" ;;
  esac
done

for source in "$ROOT"/tests/analysis/*.c; do
  case "$(basename "$source")" in
    main.c) ;;
    control_flow.c|nested_shadowing.c|shadowing.c)
      expect_compile "$source" ;;
    duplicate_parameter.c|out_of_scope.c|undeclared_variable.c|use_before_declaration.c)
      expect_reject "$source" ;;
    *)
      echo "FAIL $source: no expected result defined" >&2
      exit 1 ;;
  esac
done

for directory in ir_builder optimizer; do
  for source in "$ROOT"/tests/$directory/*.c; do
    case "$(basename "$source")" in
      main.c|runtime.c) ;;
      *) expect_compile "$source" ;;
    esac
  done
done

for expected in "$ROOT"/tests/optimizer/*.expected.ll; do
  input=${expected%.expected.ll}.ll
  actual="$TMP_DIR/$(basename "$input" .ll).actual.ll"

  if [[ ! -f $input ]]; then
    echo "FAIL $expected: optimizer input is missing" >&2
    exit 1
  fi

  "$OPTIMIZER_RUNNER" "$input" "$actual"
  if ! "$LLVM_DIFF" "$expected" "$actual"; then
    echo "FAIL $input: optimized IR does not match $expected" >&2
    exit 1
  fi
  echo "PASS $input (optimized IR matched)"
done

run_backend() {
  local source=$1
  local name
  local assembly
  local executable
  local input_file="${source%.c}.input"
  local expected="${source%.c}.expected"
  local actual

  name=$(basename "$source" .c)
  assembly="$TMP_DIR/$name.s"
  executable="$TMP_DIR/$name"
  actual="$TMP_DIR/$name.actual"

  "$COMPILER" "$source" "$assembly"
  "$CC" -m32 "$assembly" "$RUNTIME" -o "$executable"

  if [[ ! -f $expected ]]; then
    echo "FAIL $source: expected output is missing" >&2
    return 1
  fi

  if [[ -f $input_file ]]; then
    "$executable" <"$input_file" >"$actual"
  else
    "$executable" </dev/null >"$actual"
  fi

  if ! diff -u "$expected" "$actual"; then
    echo "FAIL $source: output does not match $expected" >&2
    return 1
  fi
  echo "PASS $source (output matched)"
}

for source in "$ROOT"/tests/backend/*.c; do
  case "$(basename "$source")" in
    main.c|runtime.c) ;;
    *) run_backend "$source" ;;
  esac
done

echo 'All tests passed.'
