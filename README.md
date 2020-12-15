# clang-function-prototype
>A clang's plugin to extract function's prototype.

This will extract the AST from your source codes and **no need to compile your code!**. 
What you need just is running a simple command line.

## Features
Now, it supports extracting the follow types of function prototypes
- **caller-callee** relationships
- **indirect function call** prototypes

## Environment
- clang 10

## How to install the plugin
### Build
If you want to build this plugin by yourself, replace the file `PrintFunctionNames.cpp` in directory `llvm-project/clang/examples/PrintFunctionNames`.

Then build this plugin by `make PrintFunctionNames` command. And then, the `PrintFunctionNames.so` will apperarent at `llvm-project/build/lib`.

### Use as a library

`clang -fsyntax-only -Xclang -load -Xclang ./PrintFunctionNames.so -Xclang -plugin -Xclang print-fns -Xclang -plugin-arg-print-fns -Xclang ./log.file input.c
`

Here replace `./log.file` to receive output and input file `input.c`.

## How to use it
For example, there is a c file named `helloworld.c` which contains:
```
# include <stdio.h>
int *Max(int, int);  //函数声明
int main(void)
{
    int* (*p)(int prt1, int ptr2);  //定义一个函数指针
    int a, b, *c;
    p = Max;  //把函数Max赋给指针变量p, 使p指向Max函数
    printf("please enter a and b:");
    scanf("%d%d", &a, &b);
    c = (*p)(a, b);  //通过函数指针调用Max函数
    printf("a = %d\nb = %d\nmax = %d\n", a, b, *c);
    return 0;
}
int *Max(int x, int y)  //定义Max函数
{
    int *z;
    if (x > y)
    {
        *z = x;
    }
    else
    {
        *z = y;
    }
    return z;
}
```


The result are bellow:
```
{'function': 'main', 'return_type': 'int', 'parms': '', 'file' :'test2.c', 'begin': [3, 1], 'end': [13, 1]}
	{'function': 'printf', 'return_type': 'int', 'params': 'const char *@__format,'}
	{'function': 'scanf', 'return_type': 'int', 'params': 'const char *@__format,'}
	{'function': 'printf', 'return_type': 'int', 'params': 'const char *@__format,'}
	{'function': 'p', 'return_type': 'int *', 'params': 'int@int,int@int,'}
{'function': 'Max', 'return_type': 'int *', 'parms': 'int@x,int@y,', 'file' :'test2.c', 'begin': [14, 1], 'end': [26, 1]}
```


        
## Referred Link
1. https://github.com/Robin-Y-Ding/PureFunction
2. Clang Tutorial: Finding declarations. https://xinhuang.github.io/posts/2014-10-19-clang-tutorial-finding-declarations.html
3. https://clang.llvm.org/docs/ClangPlugins.html
