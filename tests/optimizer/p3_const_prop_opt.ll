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
  store i32 30, ptr %c1.0, align 4
  %1 = load i32, ptr %p.0, align 4
  %2 = icmp slt i32 10, %1
  br i1 %2, label %3, label %4

return:                                           ; preds = %5
  ret i32 40

3:                                                ; preds = %entry
  store i32 30, ptr %b.0, align 4
  br label %5

4:                                                ; preds = %entry
  store i32 30, ptr %b.0, align 4
  br label %5

5:                                                ; preds = %4, %3
  store i32 40, ptr %ret, align 4
  br label %return
}
