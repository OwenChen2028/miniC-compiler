target triple = "x86_64-pc-linux-gnu"

declare void @print(i32 %0)

declare i32 @read()

define i32 @func(i32 %0) {
entry:
  %c0 = alloca i32, align 4
  %b0 = alloca i32, align 4
  %a0 = alloca i32, align 4
  %i0 = alloca i32, align 4
  %ret = alloca i32, align 4
  store i32 %0, ptr %i0, align 4
  store i32 10, ptr %a0, align 4
  store i32 15, ptr %b0, align 4
  store i32 25, ptr %c0, align 4
  store i32 5, ptr %a0, align 4
  br label %1

return:                                           ; preds = %10
  ret i32 40

1:                                                ; preds = %14, %entry
  %2 = load i32, ptr %a0, align 4
  %3 = load i32, ptr %i0, align 4
  %4 = icmp slt i32 %2, %3
  br i1 %4, label %5, label %10

5:                                                ; preds = %1
  %6 = load i32, ptr %a0, align 4
  %7 = add i32 %6, 1
  store i32 %7, ptr %a0, align 4
  %8 = load i32, ptr %a0, align 4
  %9 = icmp sgt i32 %8, 15
  br i1 %9, label %12, label %13

10:                                               ; preds = %1
  %11 = load i32, ptr %a0, align 4
  call void @print(i32 %11)
  call void @print(i32 15)
  call void @print(i32 25)
  store i32 40, ptr %ret, align 4
  br label %return

12:                                               ; preds = %5
  store i32 25, ptr %c0, align 4
  br label %14

13:                                               ; preds = %5
  store i32 25, ptr %c0, align 4
  br label %14

14:                                               ; preds = %13, %12
  br label %1
}
