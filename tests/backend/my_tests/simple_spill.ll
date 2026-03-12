define i32 @func(i32 %0) {
entry:
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  %c = alloca i32, align 4
  %d = alloca i32, align 4
  %ret = alloca i32, align 4

  store i32 10, ptr %a, align 4
  store i32 20, ptr %b, align 4
  store i32 30, ptr %c, align 4
  store i32 40, ptr %d, align 4

  %1 = load i32, ptr %a, align 4
  %2 = load i32, ptr %b, align 4
  %3 = load i32, ptr %c, align 4
  %4 = load i32, ptr %d, align 4

  %5 = add i32 %1, %2
  %6 = add i32 %3, %4
  %7 = add i32 %5, %6

  store i32 %7, ptr %ret, align 4
  %8 = load i32, ptr %ret, align 4
  ret i32 %8
}
