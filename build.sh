#!/bin/bash

# Universal build script for Linux, macOS, and Windows (Git Bash/WSL)

echo "========================================="
echo "Building the Mini Compiler..."
echo "========================================="

# Detect OS
OS=$(uname -s)
OUTPUT_NAME="compiler"
COMPILER=""

case "$OS" in
    Linux*)
        echo "Platform: Linux"
        COMPILER="g++"
        ;;
    Darwin*)
        echo "Platform: macOS"
        # Try clang++ first (native on Mac), fallback to g++
        if command -v clang++ &> /dev/null; then
            COMPILER="clang++"
            echo "Using clang++"
        else
            COMPILER="g++"
            echo "Using g++"
        fi
        ;;
    MINGW*|MSYS*|CYGWIN*)
        echo "Platform: Windows (Git Bash/WSL)"
        COMPILER="g++"
        OUTPUT_NAME="compiler.exe"
        ;;
    *)
        echo "Platform: Unknown (assuming g++)"
        COMPILER="g++"
        ;;
esac

# Check if compiler exists
if ! command -v $COMPILER &> /dev/null; then
    echo "✗ Error: $COMPILER not found!"
    echo "Install g++ or clang++ and try again."
    exit 1
fi

# Compile
$COMPILER -Wall -Wextra -o $OUTPUT_NAME main.cpp DFA.cpp PDA.cpp

if [ $? -eq 0 ]; then
    echo "✓ Build successful!"
    echo "Run: ./$OUTPUT_NAME"
    echo "========================================="
else
    echo "✗ Build failed!"
    exit 1
fi
