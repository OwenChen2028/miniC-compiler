#!/usr/bin/env bash

set -euo pipefail

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
BUILD_DIR=${BUILD_DIR:-"$ROOT/build"}
COMPILER="$BUILD_DIR/compiler.out"
CC=${CC:-clang}
LLVM_AS=${LLVM_AS:-llvm-as}
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
    invalid.c|p_bad.c) expect_reject "$source" ;;
    *) expect_compile "$source" ;;
  esac
done

for source in "$ROOT"/tests/analysis/*.c; do
  case "$(basename "$source")" in
    main.c) ;;
    *_good.c|control_flow.c|nested_shadowing.c|shadowing.c)
      expect_compile "$source" ;;
    *_bad.c|duplicate_parameter.c|out_of_scope.c|undeclared_variable.c|use_before_declaration.c)
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

for source in "$ROOT"/tests/optimizer/*.ll; do
  "$LLVM_AS" "$source" -o "$TMP_DIR/$(basename "$source" .ll).bc"
  echo "PASS $source (assembled)"
done

backend_input() {
  case "$1" in
    max_n|sum_n) printf '%s\n' '1 2 3 4' ;;
    repeated_operand) printf '%s\n' '9' ;;
    stress) printf '%s\n' '0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0' ;;
    *) return 1 ;;
  esac
}

run_backend() {
  local source=$1
  local name
  local assembly
  local executable
  local input

  name=$(basename "$source" .c)
  assembly="$TMP_DIR/$name.s"
  executable="$TMP_DIR/$name"

  "$COMPILER" "$source" "$assembly"
  "$CC" -m32 "$assembly" "$RUNTIME" -o "$executable"

  if input=$(backend_input "$name"); then
    printf '%s\n' "$input" | "$executable" >/dev/null
  else
    "$executable" </dev/null >/dev/null
  fi
  echo "PASS $source (compiled and ran)"
}

for source in "$ROOT"/tests/backend/*.c; do
  case "$(basename "$source")" in
    main.c|runtime.c) ;;
    *) run_backend "$source" ;;
  esac
done

echo 'All tests passed.'
