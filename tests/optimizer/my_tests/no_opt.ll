target triple = "i386-pc-linux-gnu"

declare void @print(i32)

declare i32 @read()

define i32 @func() {
entry:
  %alive.0 = alloca i32, align 4
  %y.0 = alloca i32, align 4
  %x.0 = alloca i32, align 4
  %ret = alloca i32, align 4
  %0 = call i32 @read()
  store i32 %0, ptr %alive.0, align 4
  %1 = load i32, ptr %alive.0, align 4
  %2 = add i32 %1, 1
  store i32 %2, ptr %y.0, align 4
  %3 = load i32, ptr %y.0, align 4
  %4 = add i32 %3, 3
  store i32 %4, ptr %x.0, align 4
  %5 = call i32 @read()
  store i32 %5, ptr %y.0, align 4
  %6 = load i32, ptr %y.0, align 4
  %7 = add i32 %6, 3
  store i32 %7, ptr %x.0, align 4
  %8 = load i32, ptr %x.0, align 4
  store i32 %8, ptr %ret, align 4
  br label %return

return:                                           ; preds = %entry
  %9 = load i32, ptr %ret, align 4
  ret i32 %9
}
