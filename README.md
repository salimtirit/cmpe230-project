# MyLang to LLVM Compiler Project

## Overview
This is a group project, which requires implementing a translator called mylang2IR for a language 
called MyLang. The language consists of integer variables, one-line statements, while-loops, and if 
compound statements. Operators in expressions are limited to *, /, +, -, and there is a function 
choose(expr1,expr2,expr3,expr4) that returns the value of one of the expressions based on the value 
of expr1. The translator needs to generate LLVM IR code for the input MyLang code.

## Implementation
To implement the translator, the project requires the use of LLVM (low-level virtual machine) 
version 3.3, which provides IR (intermediate representation) used to generate code for different 
target architectures. The project is implemented in C++, and the LLVM binaries can be downloaded 
from [the official website](https://releases.llvm.org/download.html#3.3).

## MyLang Syntax
1. Variables are integer variables, and their default value is 0.
2. Executable statements are either one-line statements, while-loops, or if compound statements. Nested while-loops or if statements are not allowed.
3. One-line statements can be either assignment statements or print statements that print the value of an expression.
4. The `choose(expr1,expr2,expr3,expr4)` function returns expr2 if expr1 is equal to 0, returns expr3 if expr1 is positive, and returns expr4 if expr1 is negative.
5. The only allowed operations in expressions are *, /, +, -. Parentheses are allowed, but unary minus operation is not supported. The operator precedence needs is implemented as in other programming languages, such as C or Java.
6. The # sign indicates the start of a comment in one line.
7. The if statement has the following format: `if (expr) {....}`
If **expr** has a nonzero value, it means true. If **expr** has zero value, it means false. There is no else part, and nested if statements are not allowed.
8. The while loop has the following format: `while (expr) {....}`
If **expr** has a nonzero value, it means true. If **expr** has zero value, it means false. There are no nested while statements.
9. The `print(id)` statement prints the value of variable id.
10. In case of a syntax error in line x, print _“Line x: syntax error”_.

## mylang2IR
mylang2IR generates low-level LLVM IR code for the input MyLang code. The generated code follows 
the LLVM IR syntax and uses the static single assignment (SSA) based representation. In assignment 
statements, variables are assigned a value once. The _alloca_ instruction is used to allocate space 
for variables and return the address of the allocation. The keywords _i8_, _i16_, and _i32_ mean 
8-bit, 16-bit, and 32-bit type respectively. The keyword _i32*_ means 32-bit pointer. Variables %ti 
(where i is an integer) are temporary variables.


## Example
Given the following code in file.my

```
n = 10
f0 = 0
f1 = 1
i = 2
while(n) {
  t = f1
  f1 = f0 +f1
  print(f1)
  f0 = t
  n = n - 1
}
```

mylang2IR will output the following IR code

```
; ModuleID = 'mylang2ir'
declare i32 @printf(i8*, ...)
@print.str = constant [4 x i8] c"%d\0A\00"

define i32 @main() {
  %n = alloca i32
  %f0 = alloca i32
  %f1 = alloca i32
  %i = alloca i32
  %t = alloca i32
  store i32 0, i32* %n
  store i32 0, i32* %f0
  store i32 0, i32* %f1
  store i32 0, i32* %i
  store i32 0, i32* %t
  store i32 10, i32* %n
  store i32 0, i32* %f0
  store i32 1, i32* %f1
  store i32 2, i32* %i
  br label %whcond

whcond:
  %t1 = load i32* %n
  %t2 = icmp ne i32 %t1, 0
  br i1 %t2, label %whbody, label %whend

whbody:
  %t3 = load i32* %f1
  store i32 %t3, i32* %t
  %t4 = load i32* %f0
  %t5 = load i32* %f1
  %t6 = add i32 %t4, %t5
  store i32 %t6, i32* %f1
  %t7 = load i32* %f1
  call i32 (i8*, ...)* @printf(i8* getelementptr ([4 x i8]* @print.str, i32 0, i32 0), i32 %t7 )
  %t8 = load i32* %t
  store i32 %t8, i32* %f0
  %t9 = load i32* %n
  %t10 = sub i32 %t9, 1
  store i32 %t10, i32* %n
  br label %whcond

whend:
  ret i32 0
}
```
The first 3 lines of code defines the module name and the prototype for the **printf** output
statement. This part is generated as it is shown in the above example.

The line starting with _call_ is for printing the value of a variable using the **printf** function.

Check the detailed explanation of the code in the [project report](https://github.com/salimtirit/Interpreter-project/blob/main/Project%20Documentation.pdf)

## Running the Program


## Contributing
If you have any suggestions or find any issues with the code, please feel free to contribute to the project by opening a pull request or creating an issue.

## Credits
This is the first project of CMPE230 Computer Systems class in Spring 2021.


