target triple = "i386-pc-linux-gnu"

declare void @print(i32)

declare i32 @read()

define i32 @func() {
entry:
  %y.0 = alloca i32, align 4
  %x.0 = alloca i32, align 4
  %ret = alloca i32, align 4
  store i32 3, ptr %y.0, align 4
  %0 = load i32, ptr %y.0, align 4
  %1 = add i32 %0, 3
  store i32 %1, ptr %x.0, align 4
  %2 = load i32, ptr %y.0, align 4
  call void @print(i32 %2)
  %3 = load i32, ptr %y.0, align 4
  %4 = add i32 %3, 3
  store i32 %4, ptr %x.0, align 4
  %5 = load i32, ptr %x.0, align 4
  store i32 %5, ptr %ret, align 4
  br label %return

return:                                           ; preds = %entry
  %6 = load i32, ptr %ret, align 4
  ret i32 %6
}
