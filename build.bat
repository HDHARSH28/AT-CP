@echo off
echo Building the Mini Compiler (C++ Version)...
g++ -Wall -o compiler.exe main.cpp DFA.cpp PDA.cpp
if %errorlevel% equ 0 (
    echo Build successful! Run compiler.exe
) else (
    echo Build failed!
)
