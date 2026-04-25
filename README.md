# Conditional and Branching Mini Compiler

This project is a C++ mini compiler for conditional branching input. It performs:

1. Lexical analysis using DFA-style state tracing.
2. Syntax analysis using PDA-style stack tracing.
3. Intermediate code generation (Three-Address Code with labels and gotos).

## What It Can Parse

Supported patterns:

1. `if (condition) { ... }`
2. `if ... else { ... }`
3. `else if` chains
4. Nested `if`
5. Assignments inside blocks, such as `x = 5;` and `y = x + 1;`
6. Conditions using operators: `> < >= <= == !=`

## What It Does Not Parse

Current limitations:

1. Top-level statements that do not start with `if`
2. Declarations like `int x = 10;`
3. Logical operators like `&&` and `||`
4. Loops (`for`, `while`), functions, arrays, strings
5. Empty block `{ }` (treated as invalid by parser)

## Project Files

1. `compiler.h` - Shared declarations for lexer/parser/codegen state
2. `DFA.cpp` - Tokenization (lexer) and DFA trace output
3. `PDA.cpp` - Parsing, PDA push/pop trace, and TAC generation
4. `main.cpp` - Driver that reads `input.txt` and writes report to `output.txt`
5. `run_tests.sh` - Linux test suite script
6. `build.bat` - Windows build script

## How to Run

### Linux

1. Compile:
   ```bash
   g++ -Wall -o compiler main.cpp DFA.cpp PDA.cpp
   ```
2. Put your source in `input.txt`.
3. Run:
   ```bash
   ./compiler
   ```
4. Open `output.txt` for lexer, parser, and codegen output.

### Windows

1. Build:
   ```bat
   build.bat
   ```
2. Put your source in `input.txt`.
3. Run:
   ```bat
   compiler.exe
   ```
4. Open `output.txt`.

## Input Example

```c
if (x > 5) {
    y = x + 1;
} else if (x == 5) {
    y = 0;
} else {
    y = x - 1;
}
```
