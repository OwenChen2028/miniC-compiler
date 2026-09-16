target triple = "i386-pc-linux-gnu"

declare void @print(i32)

declare i32 @read()

define i32 @func(i32 %0) {
entry:
  %c.0 = alloca i32, align 4
  %b.0 = alloca i32, align 4
  %a.0 = alloca i32, align 4
  %i.0 = alloca i32, align 4
  %ret = alloca i32, align 4
  store i32 %0, ptr %i.0, align 4
  store i32 10, ptr %a.0, align 4
  store i32 15, ptr %b.0, align 4
  %1 = load i32, ptr %a.0, align 4
  %2 = load i32, ptr %b.0, align 4
  %3 = add i32 %1, %2
  store i32 %3, ptr %c.0, align 4
  store i32 5, ptr %a.0, align 4
  br label %5

return:                                           ; preds = %15
  %4 = load i32, ptr %ret, align 4
  ret i32 %4

5:                                                ; preds = %24, %entry
  %6 = load i32, ptr %a.0, align 4
  %7 = load i32, ptr %i.0, align 4
  %8 = icmp slt i32 %6, %7
  br i1 %8, label %9, label %15

9:                                                ; preds = %5
  %10 = load i32, ptr %a.0, align 4
  %11 = add i32 %10, 1
  store i32 %11, ptr %a.0, align 4
  %12 = load i32, ptr %a.0, align 4
  %13 = load i32, ptr %b.0, align 4
  %14 = icmp sgt i32 %12, %13
  br i1 %14, label %22, label %23

15:                                               ; preds = %5
  %16 = load i32, ptr %a.0, align 4
  call void @print(i32 %16)
  %17 = load i32, ptr %b.0, align 4
  call void @print(i32 %17)
  %18 = load i32, ptr %c.0, align 4
  call void @print(i32 %18)
  %19 = load i32, ptr %b.0, align 4
  %20 = load i32, ptr %c.0, align 4
  %21 = add i32 %19, %20
  store i32 %21, ptr %ret, align 4
  br label %return

22:                                               ; preds = %9
  store i32 25, ptr %c.0, align 4
  br label %24

23:                                               ; preds = %9
  store i32 25, ptr %c.0, align 4
  br label %24

24:                                               ; preds = %23, %22
  br label %5
}
