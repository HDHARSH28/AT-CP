@echo off
echo Building the Mini Compiler...
gcc -Wall -o compiler.exe main.c lexer.c parser.c codegen.c printer.c
if %errorlevel% equ 0 (
    echo Build successful! Run compiler.exe
) else (
    echo Build failed!
)
