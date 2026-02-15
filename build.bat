@echo off
REM Build script for Same Game - Windows Batch

echo Building Same Game...

REM Set PATH to use MSYS2 ucrt64 first
set "PATH=C:\msys64\ucrt64\bin;%PATH%"

g++.exe -std=c++17 -IC:\msys64\ucrt64\include -I. SameGame.cpp main.cpp -o SameGame.exe -LC:\msys64\ucrt64\lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf

if %ERRORLEVEL% EQU 0 (
    echo Build successful!
    echo Run SameGame.exe to play the game.
    echo.
    echo Controls:
    echo - Mouse Hover: Highlight clusters
    echo - Left Click: Remove cluster  
    echo - R Key: Restart game
    echo - ESC Key: Quit
) else (
    echo Build failed!
    exit /b 1
)
