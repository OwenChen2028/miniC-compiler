
define i32 @func(i32 %0) {
entry:
  %a = alloca i32
  %b = alloca i32
  %c = alloca i32
  %d = alloca i32
  %e = alloca i32
  %f = alloca i32
  %g = alloca i32
  %h = alloca i32
  %i = alloca i32
  %ret = alloca i32

  store i32 1, ptr %a
  store i32 2, ptr %b
  store i32 3, ptr %c
  store i32 4, ptr %d
  store i32 5, ptr %e
  store i32 6, ptr %f
  store i32 7, ptr %g
  store i32 8, ptr %h
  store i32 9, ptr %i

  %1 = load i32, ptr %a
  %2 = load i32, ptr %b
  %3 = load i32, ptr %c
  %4 = load i32, ptr %d
  %5 = load i32, ptr %e
  %6 = load i32, ptr %f
  %7 = load i32, ptr %g
  %8 = load i32, ptr %h
  %9 = load i32, ptr %i

  %10 = add i32 %1, %2
  %11 = add i32 %3, %4
  %12 = add i32 %5, %6
  %13 = add i32 %7, %8
  %14 = add i32 %10, %11
  %15 = add i32 %12, %13
  %16 = add i32 %14, %15
  %17 = add i32 %16, %9

  store i32 %17, ptr %ret
  %18 = load i32, ptr %ret
  ret i32 %18
}
