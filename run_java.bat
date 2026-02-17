@echo off
REM Build and run script for Same Game Java version (Windows)

echo ==========================================
echo Same Game - Java Version
echo ==========================================
echo.

REM Check if Java is installed
javac -version >nul 2>&1
if errorlevel 1 (
    echo Error: Java compiler (javac) not found!
    echo Please install Java JDK 11 or higher.
    exit /b 1
)

echo Java version detected:
javac -version
echo.

REM Compile the Java files
echo Compiling Java files...
cd src\main\java
javac com\samegame\*.java

if errorlevel 1 (
    echo Compilation failed!
    exit /b 1
)

REM Check what to run
if "%1"=="test" (
    echo Running test suite...
    java -ea com.samegame.TestGraph
) else if "%1"=="clean" (
    echo Cleaning compiled files...
    del /Q com\samegame\*.class 2>nul
    echo Clean complete!
) else (
    echo Starting Same Game GUI...
    echo Controls:
    echo   - Mouse: Hover to highlight, Click to remove clusters
    echo   - R: Restart game
    echo   - ESC: Quit
    echo.
    java com.samegame.SameGameGUI
)
