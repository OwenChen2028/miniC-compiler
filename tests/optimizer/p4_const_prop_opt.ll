target triple = "i386-pc-linux-gnu"

declare void @print(i32)

declare i32 @read()

define i32 @func(i32 %0) {
entry:
  %d.0 = alloca i32, align 4
  %c.0 = alloca i32, align 4
  %b.0 = alloca i32, align 4
  %a.0 = alloca i32, align 4
  %i.0 = alloca i32, align 4
  %ret = alloca i32, align 4
  store i32 %0, ptr %i.0, align 4
  store i32 10, ptr %a.0, align 4
  store i32 20, ptr %b.0, align 4
  store i32 20, ptr %c.0, align 4
  store i32 5, ptr %a.0, align 4
  br label %2

return:                                           ; preds = %11
  %1 = load i32, ptr %ret, align 4
  ret i32 %1

2:                                                ; preds = %17, %entry
  %3 = load i32, ptr %a.0, align 4
  %4 = load i32, ptr %i.0, align 4
  %5 = icmp slt i32 %3, %4
  br i1 %5, label %6, label %11

6:                                                ; preds = %2
  %7 = load i32, ptr %a.0, align 4
  %8 = add i32 %7, 1
  store i32 %8, ptr %a.0, align 4
  %9 = load i32, ptr %a.0, align 4
  %10 = icmp sgt i32 %9, 20
  br i1 %10, label %15, label %16

11:                                               ; preds = %2
  %12 = load i32, ptr %a.0, align 4
  call void @print(i32 %12)
  call void @print(i32 20)
  %13 = load i32, ptr %c.0, align 4
  call void @print(i32 %13)
  %14 = add i32 20, %13
  store i32 %14, ptr %ret, align 4
  br label %return

15:                                               ; preds = %6
  store i32 25, ptr %c.0, align 4
  br label %17

16:                                               ; preds = %6
  store i32 25, ptr %c.0, align 4
  br label %17

17:                                               ; preds = %16, %15
  br label %2
}
