define i32 @func(i32 %0) {
entry:
  %a = alloca i32
  %b = alloca i32
  %c = alloca i32
  %d = alloca i32
  %ret = alloca i32

  store i32 1, ptr %a
  store i32 2, ptr %b
  store i32 3, ptr %c
  store i32 4, ptr %d

  br label %block1

block1:
  %1 = load i32, ptr %a
  %2 = load i32, ptr %b
  %3 = load i32, ptr %c
  %4 = load i32, ptr %d

  %5 = add i32 %1, %2
  br label %block2

block2:
  %6 = add i32 %3, %4
  %7 = add i32 %5, %6

  store i32 %7, ptr %ret
  %8 = load i32, ptr %ret
  ret i32 %8
}
