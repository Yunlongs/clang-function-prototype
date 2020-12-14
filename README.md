# clang-function-prototype
>A clang's plugin to extract function's prototype.

This will extract the AST from your source codes and **no need to compile your code!**. 
What you need just is running a simple command line.

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


The result are bellow:
```
{'function': 'func', 'return_type': 'void', 'parms': '', 'file' :'test.c', 'begin': [3, 1], 'end': [7, 1]}
{'function': 'main', 'return_type': 'int', 'parms': '', 'file' :'test.c', 'begin': [9, 1], 'end': [13, 1]}
{'function': 'printf', 'return_type': 'int', 'parms': 'const char *@__format,'}
```
>The error is not important, because we parse the current file's AST don't need include files. That also means you can extract function prototype with a separated file, although it include some relevant files.

        
## Referred Link
1. https://github.com/Robin-Y-Ding/PureFunction
2. Clang Tutorial: Finding declarations. https://xinhuang.github.io/posts/2014-10-19-clang-tutorial-finding-declarations.html
3. https://clang.llvm.org/docs/ClangPlugins.html
