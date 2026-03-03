@echo off
REM Run script for Same Game - Windows Batch
REM Adds MSYS2 MinGW64 bin to PATH so SDL2 DLLs can be found

echo Preparing to launch Same Game...

REM Set PATH to include MSYS2 mingw64 bin directory
set "PATH=C:\msys64\mingw64\bin;%PATH%"

REM Check if SameGame.exe exists
if not exist "SameGame.exe" (
    echo Error: SameGame.exe not found! Please run build.bat first.
    pause
    exit /b 1
)

echo Starting SameGame.exe...
start "" "SameGame.exe"

if %ERRORLEVEL% EQU 0 (
    echo Game launched successfully.
) else (
    echo Failed to launch game!
)
