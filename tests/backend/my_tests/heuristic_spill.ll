define i32 @func(i32 %0) {
entry:
  %a = alloca i32
  %b = alloca i32
  %c = alloca i32
  %d = alloca i32
  %e = alloca i32
  %ret = alloca i32

  store i32 1, ptr %a
  store i32 2, ptr %b
  store i32 3, ptr %c
  store i32 4, ptr %d
  store i32 5, ptr %e

  %1 = load i32, ptr %a
  %2 = load i32, ptr %b
  %3 = load i32, ptr %c
  %4 = load i32, ptr %d
  %5 = load i32, ptr %e

  %6 = add i32 %1, %2
  %7 = add i32 %3, %4
  %8 = add i32 %6, %7
  %9 = add i32 %8, %5

  store i32 %9, ptr %ret
  %10 = load i32, ptr %ret
  ret i32 %10
}
