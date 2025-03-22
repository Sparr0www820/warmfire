# Warmfire Programming Language (C++ Written Compiler)
 A self-designed programming language compiler that compiles '.wf' into x86-64 asm and generate executable. Written in C++.

## Requirements
- Linux (Ubuntu 22.04 and later is recommended)
- gcc

## Building
### Linux
```bash
git clone https://github.com/Sparr0www820/warmfire
cd warmfire
mkdir build
cmake -S . -B build
cmake --build build
```
The executable `warmfire` will be produced in the `build` directory.

## Usage
### Linux
```bash
cd build
./warmfire <file.wf> [--clean]
```
The `--clean` option will remove temporary files after compilation. Check `docs/warmfire.md` for current documentation, and use `demo.wf` for a quick start.

## Contributors
None. Any contributions will be appreciated.
