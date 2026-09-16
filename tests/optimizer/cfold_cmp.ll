target triple = "i386-pc-linux-gnu"

declare void @print(i32)

declare i32 @read()

define i32 @func(i32 %0) {
entry:
  %b.0 = alloca i32, align 4
  %a.0 = alloca i32, align 4
  %p.0 = alloca i32, align 4
  %ret = alloca i32, align 4
  store i32 %0, ptr %p.0, align 4
  store i32 10, ptr %a.0, align 4
  store i32 20, ptr %b.0, align 4
  %1 = load i32, ptr %a.0, align 4
  %2 = load i32, ptr %b.0, align 4
  %3 = icmp slt i32 %1, %2
  br i1 %3, label %5, label %6

return:                                           ; preds = %6, %5
  %4 = load i32, ptr %ret, align 4
  ret i32 %4

5:                                                ; preds = %entry
  store i32 1, ptr %ret, align 4
  br label %return

6:                                                ; preds = %entry
  store i32 0, ptr %ret, align 4
  br label %return
}
