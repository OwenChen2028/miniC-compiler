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
  br i1 %2, label %3, label %4

return:                                           ; preds = %9
  ret i32 1

3:                                                ; preds = %entry
  call void @print(i32 1)
  store i32 1, ptr %x.0, align 4
  store i32 0, ptr %y.0, align 4
  br label %5

4:                                                ; preds = %entry
  store i32 1, ptr %x.0, align 4
  store i32 1, ptr %y.0, align 4
  br label %5

5:                                                ; preds = %4, %3
  %6 = load i32, ptr %y.0, align 4
  %7 = icmp eq i32 %6, 1
  br i1 %7, label %8, label %9

8:                                                ; preds = %5
  store i32 1, ptr %x.0, align 4
  store i32 2, ptr %y.0, align 4
  br label %9

9:                                                ; preds = %8, %5
  store i32 1, ptr %ret, align 4
  br label %return
}
