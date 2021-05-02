; ModuleID = 'mylang2ir'
declare i32 @printf(i8*, ...)
@print.str = constant [4 x i8] c"%d\0A\00"
@print.str1 = constant [23 x i8] c"Line %d: syntax error\0A\00"
define i32 @main() {
call i32 (i8*, ...)* @printf(i8* getelementptr ([23 x i8]* @print.str1, i32 0, i32 0), i32 9 )
ret i32 0
}
