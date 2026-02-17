# Same Game

A tile-matching puzzle game with both **C++** and **Java** implementations. The game features a graph-based approach for tile management and includes a computer AI opponent using a greedy algorithm.

## 🎮 Game Overview

Same Game is a puzzle game where you remove clusters of adjacent tiles of the same color. The game ends when no more moves are possible, and the player with the highest score wins!

### Game Rules

1. Click on a cluster of 2+ adjacent tiles of the same color to remove them
2. Score is calculated as: `(cluster_size - 2)²`
3. After removing tiles:
   - Remaining tiles fall down (vertical gravity)
   - Empty columns shift left (horizontal gravity)
4. Player and computer take turns
5. Game ends when no valid moves remain
6. Highest score wins!

### Colors
- 🟢 Green
- ⚪ White
- 🔴 Red
- 🔵 Blue
- 🟡 Yellow

## 📁 Two Complete Implementations

This repository contains **two complete, equivalent implementations**:

### 1. C++ Version (Original)
- **GUI**: SDL2 with SDL_ttf
- **Files**: `SameGame.h`, `SameGame.cpp`, `main.cpp`, `test_graph.cpp`
- **Build**: MinGW/g++ with SDL2 libraries
- **Documentation**: `BUILD.md`

### 2. Java Version (Converted)
- **GUI**: Java Swing/AWT
- **Files**: `src/main/java/com/samegame/*.java`
- **Build**: Maven or javac (no external dependencies!)
- **Documentation**: `BUILD_JAVA.md`, `README_JAVA.md`

## 🚀 Quick Start

### Run C++ Version

```bash
# Install SDL2 and SDL2_ttf first (see BUILD.md)
g++ -std=c++17 -I. SameGame.cpp main.cpp -o SameGame.exe -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf
./SameGame.exe
```

### Run Java Version

```bash
# Option 1: Using the run script (Unix/Linux/Mac)
./run_java.sh

# Option 2: Using the run script (Windows)
run_java.bat

# Option 3: Using Maven
mvn exec:java -Dexec.mainClass="com.samegame.SameGameGUI"

# Option 4: Manual compilation
cd src/main/java
javac com/samegame/*.java
java com.samegame.SameGameGUI
```

## 🎯 Features

Both versions include:

- ✅ **Graph-based tile system** - Efficient adjacency tracking
- ✅ **BFS cluster detection** - Find connected tiles of same color
- ✅ **Gravity system** - Vertical drop + horizontal collapse
- ✅ **Turn-based gameplay** - User vs Computer
- ✅ **Greedy AI** - Computer opponent picks largest clusters
- ✅ **Visual highlighting** - See clusters before clicking
- ✅ **Real-time scoring** - Track both players' scores
- ✅ **Game over detection** - Automatic win/loss determination
- ✅ **Restart functionality** - Press R for new game

## 🎮 Controls

Both versions use identical controls:

| Input | Action |
|-------|--------|
| **Mouse Hover** | Highlight cluster |
| **Left Click** | Remove cluster |
| **R Key** | Restart game |
| **ESC Key** | Quit |

## 📚 Documentation

- **[BUILD.md](BUILD.md)** - C++ build instructions with SDL2 setup
- **[BUILD_JAVA.md](BUILD_JAVA.md)** - Comprehensive Java build guide
- **[README_JAVA.md](README_JAVA.md)** - Java version overview and comparison
- **[GRAPH_STRUCTURE.md](GRAPH_STRUCTURE.md)** - Algorithm and data structure details

## 🔄 C++ to Java Conversion

The Java version is a **direct, faithful conversion** of the C++ code:

### Technology Mappings

| Aspect | C++ | Java |
|--------|-----|------|
| **GUI** | SDL2 + SDL_ttf | Java Swing + AWT |
| **Containers** | `vector`, `unordered_map` | `ArrayList`, `HashMap` |
| **Memory** | Manual (pointers) | Automatic (GC) |
| **Build** | g++/MinGW | Maven/javac |
| **Dependencies** | SDL2, SDL2_ttf | None (Swing is built-in) |
| **Platform** | Requires recompilation | Write once, run anywhere |

### Why Two Versions?

1. **Educational**: Shows how to convert C++ to Java
2. **Accessibility**: Java version requires no external libraries
3. **Cross-platform**: Java runs everywhere without recompilation
4. **Comparison**: Learn differences between C++ and Java
5. **Choice**: Use whichever technology stack you prefer

## 🧪 Testing

Both versions include comprehensive test suites:

```bash
# C++ tests
g++ -std=c++17 SameGame.cpp test_graph.cpp -o test_graph
./test_graph

# Java tests
cd src/main/java
javac com/samegame/*.java
java -ea com.samegame.TestGraph
```

All tests validate:
- Grid initialization
- Tile operations
- Cluster detection
- Gravity mechanics
- Score calculation
- Turn-based gameplay
- AI functionality

## 📂 Project Structure

```
Same-Game/
├── C++ Implementation
│   ├── SameGame.h              # Game logic header
│   ├── SameGame.cpp            # Game logic implementation
│   ├── main.cpp                # SDL2 GUI
│   ├── test_graph.cpp          # Test suite
│   ├── greedy.cpp              # Console prototype
│   ├── build.bat               # Windows build script
│   └── build.sh                # Unix build script
│
├── Java Implementation
│   ├── src/main/java/com/samegame/
│   │   ├── Node.java           # Graph node
│   │   ├── SameGame.java       # Game logic
│   │   ├── SameGameGUI.java    # Swing GUI
│   │   └── TestGraph.java      # Test suite
│   ├── pom.xml                 # Maven configuration
│   ├── run_java.sh             # Unix run script
│   └── run_java.bat            # Windows run script
│
├── Documentation
│   ├── README.md               # This file
│   ├── README_JAVA.md          # Java-specific readme
│   ├── BUILD.md                # C++ build guide
│   ├── BUILD_JAVA.md           # Java build guide
│   └── GRAPH_STRUCTURE.md      # Algorithm documentation
│
└── Configuration
    ├── .gitignore              # Git ignore rules
    └── build_output.txt        # Build logs
```

## 🛠️ Development

### Prerequisites

**For C++ version:**
- C++17 compiler (g++/MinGW)
- SDL2 development libraries
- SDL2_ttf library

**For Java version:**
- JDK 11 or higher
- Maven (optional)

### Building

See language-specific build documentation:
- C++: [BUILD.md](BUILD.md)
- Java: [BUILD_JAVA.md](BUILD_JAVA.md)

## 🎯 Algorithm Overview

Both implementations use identical algorithms:

1. **Graph Representation**: Each tile is a node with edges to adjacent tiles
2. **Cluster Detection**: BFS traversal finds connected same-color tiles
3. **Gravity**: Two-phase system
   - Phase 1: Vertical - tiles fall down
   - Phase 2: Horizontal - empty columns shift left
4. **AI Strategy**: Greedy algorithm always picks largest available cluster

For detailed algorithm explanation, see [GRAPH_STRUCTURE.md](GRAPH_STRUCTURE.md)

## 📈 Performance

Both versions provide excellent performance:
- **C++**: Hardware-accelerated SDL2 rendering
- **Java**: Smooth Swing double-buffered rendering
- Both: ~60 FPS, instant cluster detection
- Grid size: 6×8 (48 tiles)

## 🤝 Contributing

Contributions are welcome! When adding features:
1. Maintain consistency between C++ and Java versions
2. Ensure all tests pass
3. Follow existing code style
4. Update relevant documentation

## 📄 License

Open source project. See individual files for licensing details.

## 🎓 Learning Resources

This project demonstrates:
- Graph-based game design
- BFS algorithm implementation
- GUI programming (SDL2 and Swing)
- Cross-language code conversion
- Turn-based game logic
- AI opponent implementation

## 🙏 Acknowledgments

- Original C++ implementation with SDL2
- Direct Java conversion maintaining all features
- Graph-based tile system design
- Greedy AI algorithm

---

**Choose your preferred version and enjoy the game! 🎮**
