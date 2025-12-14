# Same Game - Build Instructions

## Prerequisites
You need to install SDL2 and SDL2_ttf libraries for Windows/MinGW:

### Option 1: Using MSYS2 (Recommended)
1. Install MSYS2 from https://www.msys2.org/
2. Open MSYS2 MinGW 64-bit terminal
3. Run: `pacman -S mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_ttf`

### Option 2: Manual Installation
1. Download SDL2 development libraries (MinGW) from https://www.libsdl.org/download-2.0.php
2. Download SDL2_ttf from https://www.libsdl.org/projects/SDL_ttf/
3. Extract and add to your MinGW include/lib directories

## Build Commands

### Compile the project:
```bash
g++ -std=c++17 -I. SameGame.cpp main.cpp -o SameGame.exe -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf
```

### Run:
```bash
./SameGame.exe
```

## Controls
- **Mouse Hover**: Highlight clusters
- **Left Click**: Remove cluster
- **R Key**: Restart game
- **ESC Key**: Quit

## File Structure
- `SameGame.h` - Game logic class header
- `SameGame.cpp` - Game logic implementation
- `main.cpp` - SDL2 GUI implementation
- `greedy.cpp` - Original console version (deprecated)
