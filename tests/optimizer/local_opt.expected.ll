target triple = "i386-pc-linux-gnu"

declare void @print(i32)

declare i32 @read()

define i32 @func() {
entry:
  %y.0 = alloca i32, align 4
  %x.0 = alloca i32, align 4
  %ret = alloca i32, align 4
  store i32 3, ptr %y.0, align 4
  store i32 6, ptr %x.0, align 4
  call void @print(i32 3)
  store i32 6, ptr %x.0, align 4
  store i32 6, ptr %ret, align 4
  br label %return

return:                                           ; preds = %entry
  ret i32 6
}
