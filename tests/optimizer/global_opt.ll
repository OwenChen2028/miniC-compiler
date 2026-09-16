target triple = "i386-pc-linux-gnu"

declare void @print(i32)

declare i32 @read()

define i32 @func() {
entry:
  %y.0 = alloca i32, align 4
  %x.0 = alloca i32, align 4
  %ret = alloca i32, align 4
  store i32 1, ptr %x.0, align 4
  %0 = call i32 @read()
  store i32 %0, ptr %y.0, align 4
  %1 = load i32, ptr %y.0, align 4
  %2 = icmp eq i32 %1, 1
  br i1 %2, label %4, label %6

return:                                           ; preds = %14
  %3 = load i32, ptr %ret, align 4
  ret i32 %3

4:                                                ; preds = %entry
  %5 = load i32, ptr %x.0, align 4
  call void @print(i32 %5)
  store i32 1, ptr %x.0, align 4
  store i32 0, ptr %y.0, align 4
  br label %8

6:                                                ; preds = %entry
  store i32 1, ptr %x.0, align 4
  %7 = load i32, ptr %x.0, align 4
  store i32 %7, ptr %y.0, align 4
  br label %8

8:                                                ; preds = %6, %4
  %9 = load i32, ptr %y.0, align 4
  %10 = icmp eq i32 %9, 1
  br i1 %10, label %11, label %14

11:                                               ; preds = %8
  store i32 1, ptr %x.0, align 4
  %12 = load i32, ptr %x.0, align 4
  %13 = add i32 %12, 1
  store i32 %13, ptr %y.0, align 4
  br label %14

14:                                               ; preds = %11, %8
  %15 = load i32, ptr %x.0, align 4
  store i32 %15, ptr %ret, align 4
  br label %return
}
