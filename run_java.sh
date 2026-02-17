#!/bin/bash
# Build and run script for Same Game Java version

set -e

echo "=========================================="
echo "Same Game - Java Version"
echo "=========================================="
echo ""

# Check if Java is installed
if ! command -v javac &> /dev/null; then
    echo "Error: Java compiler (javac) not found!"
    echo "Please install Java JDK 11 or higher."
    exit 1
fi

echo "Java version detected:"
javac -version
echo ""

# Compile the Java files
echo "Compiling Java files..."
cd src/main/java
javac com/samegame/*.java

# Check what to run
if [ "$1" == "test" ]; then
    echo "Running test suite..."
    java -ea com.samegame.TestGraph
elif [ "$1" == "clean" ]; then
    echo "Cleaning compiled files..."
    rm -f com/samegame/*.class
    echo "Clean complete!"
else
    echo "Starting Same Game GUI..."
    echo "Controls:"
    echo "  - Mouse: Hover to highlight, Click to remove clusters"
    echo "  - R: Restart game"
    echo "  - ESC: Quit"
    echo ""
    java com.samegame.SameGameGUI
fi
