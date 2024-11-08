This is an example project using CMake.

The requirements are:

- CMake 3.14+
- A C++11 compatible compiler 
- Git
- Doxygen (optional)

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
./cynotestlib --reporter compact --success
```

