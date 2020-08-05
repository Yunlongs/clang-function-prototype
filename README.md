# clang-function-prototype
>A clang's plugin to extract function's prototype.

This will extract the AST from your source codes and **no need to compile your code!**. 
What you need just is running a simple command line.

## How to install the plugin
### Step 1
Create a directory named `clang-function-prototype` in the directory:`llvm-src/tools/clang/tools/`.

### Step 2
Copy the files `FindFunction.cpp & CMakeLists.txt` to the `clang-function-prototype` dir.

### Step 3
Add line `add_clang_subdirectory(clang-function-prototype)` into `llvm-src/tools/clang/tools/CMakeLists.txt`

### Step 4
In your llvm build directory build your llvm-src again.
And you will get a binary named `clang-function-prototype` in `llvm_build/bin/`

## How to use it
For example, there is a c file named `helloworld.c` which contains:
```
#include<stdio.h>

void func()
{
	int a = 1;
	return;
}

int main()
{
	printf("hello world!");
	return 0;
}
```

Then just run the follow command line:
`./clang-function-prototype helloworld.c`

The result are bellow:
```
Error while trying to load a compilation database:
Could not auto-detect compilation database for file "helloworld.c"
No compilation database found in /home/yunlong/llvm/llvm_release or any parent directory
fixed-compilation-database: Error while opening fixed database: No such file or directory
json-compilation-database: Error while opening JSON database: No such file or directory
Running without flags.
In file included from /home/yunlong/llvm/llvm_release/helloworld.c:1:
/usr/include/stdio.h:33:10: fatal error: 'stddef.h' file not found
#include <stddef.h>
         ^~~~~~~~~~
{'function': 'func', 'return_type': 'void', 'parms': '', 'file': '/home/yunlong/llvm/llvm_release/helloworld.c', 'begin': [3, 1], 'end': [7, 1]}
{'function': 'main', 'return_type': 'int', 'parms': '', 'file': '/home/yunlong/llvm/llvm_release/helloworld.c', 'begin': [9, 1], 'end': [13, 1]}
1 error generated.
Error while processing /home/yunlong/llvm/llvm_release/helloworld.c.
```
>The error is not important, because we parse the current file's AST don't need include files. That also means you can extract function prototype with a separated file, although it include some relevant files.

## Output file
The default outfile is `prototype.result` which memorys the output.
And you can see that the output format is json,so that you can load this file easily.

## Notice
I have tried servel c files in openssl, and you can see this file in directory `test`. I find that the plugin could ignore some functions casually.

For example, the `apps.c`, it can't identify the function between line `1151` and `3000`. 

I have tried many way to fix it, but i failed. If you have some ideas, please help me.

**But,it can work properly at most time, don't worry.**

## Referred Link
1. https://github.com/Robin-Y-Ding/PureFunction
2. Clang Tutorial: Finding declarations. https://xinhuang.github.io/posts/2014-10-19-clang-tutorial-finding-declarations.html
