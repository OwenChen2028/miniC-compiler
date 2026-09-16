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
  %1 = load i32, ptr %a.0, align 4
  %2 = add i32 %1, 10
  store i32 %2, ptr %c.0, align 4
  store i32 5, ptr %a.0, align 4
  br label %4

return:                                           ; preds = %14
  %3 = load i32, ptr %ret, align 4
  ret i32 %3

4:                                                ; preds = %23, %entry
  %5 = load i32, ptr %a.0, align 4
  %6 = load i32, ptr %i.0, align 4
  %7 = icmp slt i32 %5, %6
  br i1 %7, label %8, label %14

8:                                                ; preds = %4
  %9 = load i32, ptr %a.0, align 4
  %10 = add i32 %9, 1
  store i32 %10, ptr %a.0, align 4
  %11 = load i32, ptr %a.0, align 4
  %12 = load i32, ptr %b.0, align 4
  %13 = icmp sgt i32 %11, %12
  br i1 %13, label %21, label %22

14:                                               ; preds = %4
  %15 = load i32, ptr %a.0, align 4
  call void @print(i32 %15)
  %16 = load i32, ptr %b.0, align 4
  call void @print(i32 %16)
  %17 = load i32, ptr %c.0, align 4
  call void @print(i32 %17)
  %18 = load i32, ptr %b.0, align 4
  %19 = load i32, ptr %c.0, align 4
  %20 = add i32 %18, %19
  store i32 %20, ptr %ret, align 4
  br label %return

21:                                               ; preds = %8
  store i32 25, ptr %c.0, align 4
  br label %23

22:                                               ; preds = %8
  store i32 25, ptr %c.0, align 4
  br label %23

23:                                               ; preds = %22, %21
  br label %4
}
