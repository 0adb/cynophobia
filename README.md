# cynophobia: a compiler for a subset of C17

The design and implementation of this compiler will be based on [Nora Sandler](https://github.com/nlsandler)'s book ["Writing a C Compiler"](https://nostarch.com/writing-c-compiler).

## Setup details

To build this repo from source, you'll need:

- CMake 3.14+
- A C++11 compatible compiler 
- Git 

To configure CMake, run:

```bash
cmake -S . -B build
```

To build the project after configuration, run:

```bash
cmake --build build
```

To run the current test file (after build):
```bash
cd tests
./cynotester
```

## Status

The last working commit has passed tests written by Sandler for Chapter 1's lexing stage, available at [this repository](https://github.com/nlsandler/writing-a-c-compiler-tests).
