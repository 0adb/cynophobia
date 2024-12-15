

The requirements are:

- CMake 3.14+
- A C++11 compatible compiler 
- Git 

To configure:

```bash
cmake -S . -B build
```
 

To build:

```bash
cmake --build build
```

To run the current test file (after build):
```bash
cd tests
./cynotester
```


------
Current progress: 
tests in https://github.com/nlsandler/writing-a-c-compiler-tests/ 
[+] passes chapter 1, lexer
[ ] passes chapter 1, rest of stages
