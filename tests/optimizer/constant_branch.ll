target triple = "i386-pc-linux-gnu"

declare void @print(i32)

declare i32 @read()

define i32 @func(i32 %0) {
entry:
  %b.0 = alloca i32, align 4
  %c1.0 = alloca i32, align 4
  %a.0 = alloca i32, align 4
  %p.0 = alloca i32, align 4
  %ret = alloca i32, align 4
  store i32 %0, ptr %p.0, align 4
  store i32 10, ptr %a.0, align 4
  store i32 20, ptr %b.0, align 4
  %1 = load i32, ptr %a.0, align 4
  %2 = load i32, ptr %b.0, align 4
  %3 = add i32 %1, %2
  store i32 %3, ptr %c1.0, align 4
  %4 = load i32, ptr %a.0, align 4
  %5 = load i32, ptr %p.0, align 4
  %6 = icmp slt i32 %4, %5
  br i1 %6, label %8, label %9

return:                                           ; preds = %11
  %7 = load i32, ptr %ret, align 4
  ret i32 %7

8:                                                ; preds = %entry
  store i32 30, ptr %b.0, align 4
  br label %11

9:                                                ; preds = %entry
  %10 = load i32, ptr %c1.0, align 4
  store i32 %10, ptr %b.0, align 4
  br label %11

11:                                               ; preds = %9, %8
  %12 = load i32, ptr %b.0, align 4
  %13 = load i32, ptr %a.0, align 4
  %14 = add i32 %12, %13
  store i32 %14, ptr %ret, align 4
  br label %return
}
