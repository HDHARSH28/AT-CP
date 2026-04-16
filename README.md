# Conditional and Branching Mini Compiler

A pure C implementation of a mini-compiler that analyzes conditional and branching statements (`if`, `if-else`). It performs lexical analysis, syntax validation, and generates intermediate branching code (Three-Address Code).

This project relies purely on standard C constructs and avoids complicated syntaxes or dependencies, making it very easy to explain and trace the exact automaton steps.

## Automata Theory Connections (Syllabus Mapping)

1. **Unit 1 & 2 (FA & Regular Languages)**
   - The Lexer is built as a pure **DFA (Deterministic Finite Automata) Simulator**.
   - It traces character-by-character changes across states (e.g. `S0 -> S1 (word) -> ACCEPT`).
2. **Unit 3 & 4 (CFG & Pushdown Automata)**
   - The Parser uses a simulated explicit **PDA stack**.
   - It logs the `PUSH` and `POP` states dynamically when checking the bounds of `()`, `{}`, and Grammar matches.
   - Outputs the exact **CFG Leftmost Derivation steps** and an Abstract Syntax Tree (AST).
3. **Unit 5 & 6 (Code Generation/Translation)**
   - Intermediate TAC (Three Address Code) translates the AST into conditional `goto` jumps, simulating assembly control flow.

## Files Structure

Because the code is kept remarkably simple, it is highly modularized for readability:

- **`compiler.h`** — The only header file. Contains constants, data structures (Token, ASTNode limits), and external global variable definitions.
- **`lexer.c`** — Phase 1: DFA logic (`tokenize`).
- **`parser.c`** — Phase 2: PDA and Syntax verification logic (`parseProgram`).
- **`codegen.c`** — Phase 3: TAC conversion logic (`generateCode`).
- **`printer.c`** — Formatted textual trace output utilities so they don't clog up algorithm logic.
- **`main.c`** — The entry engine. It reads from files, triggers the 3 phases, and processes the final outputs.
- **`build.bat`** — An easy script to automatically compile your compiler in Windows.

## Supported Inputs

The compiler understands variable declarations, assignments, logic and math operators inside branching logic.
```c
int x = 10;
if (x > 5 && x != 10) {
    x = x + 1;
} else if (x == 5 || x < 0) {
    x = 0;
} else {
    x = x - 1;
}
```

## How to Build and Run (Windows)

1. Double-click **`build.bat`** (or run `.\build.bat` in a terminal) to compile everything effortlessly using `gcc`.
2. Write your source code in **`input.txt`**.
3. Run the executable:
   ```bash
   .\compiler.exe
   ```
4. Look at **`output.txt`**. It will contain a very rich trace:
   - Your source code
   - Lexical Analysis Result Table
   - DFA State Transitions Trace
   - PDA Stack Push/Pop Trace
   - Leftmost Grammar Derivation
   - Plotted Abstract Syntax Tree
   - Three-Address Code output
