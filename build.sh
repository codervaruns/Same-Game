#!/bin/bash
# Build script for Same Game

# Add MSYS2 MinGW64 to PATH
export PATH="/c/msys64/mingw64/bin:$PATH"

# Build the project
g++ -std=c++17 \
    -I/c/msys64/mingw64/include \
    -I. \
    SameGame.cpp main.cpp \
    -o SameGame.exe \
    -L/c/msys64/mingw64/lib \
    -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf

if [ $? -eq 0 ]; then
    echo "Build successful! Run ./SameGame.exe to play."
else
    echo "Build failed!"
    exit 1
fi
