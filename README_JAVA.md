# Same Game - Java Version

A complete Java conversion of the C++ Same Game implementation, featuring a Java Swing GUI instead of SDL2.

## Overview

This is a direct conversion from C++ to Java, maintaining the same game logic, structure, and algorithms. The project demonstrates how to translate C++ code to Java while preserving functionality.

## What's Converted

### Files Converted:
1. **SameGame.h/cpp** → **SameGame.java** + **Node.java**
   - Graph-based game logic
   - BFS cluster detection
   - Gravity system
   - Turn-based gameplay
   - Greedy AI algorithm

2. **main.cpp (SDL2 GUI)** → **SameGameGUI.java (Swing GUI)**
   - Complete GUI implementation
   - Mouse interaction
   - Visual rendering
   - Game loop
   - Event handling

3. **test_graph.cpp** → **TestGraph.java**
   - Comprehensive test suite
   - All original tests maintained

## Technology Stack

- **Language**: Java 11+
- **GUI Framework**: Java Swing/AWT (instead of SDL2)
- **Build Tool**: Maven
- **No external dependencies** (Swing is part of JDK)

## Quick Start

### Prerequisites
- Java JDK 11 or higher
- Maven (optional, for building)

### Compile and Run

#### Using Maven:
```bash
# Compile
mvn clean compile

# Run the game
mvn exec:java -Dexec.mainClass="com.samegame.SameGameGUI"

# Run tests
mvn exec:java -Dexec.mainClass="com.samegame.TestGraph"
```

#### Using javac directly:
```bash
# Compile
cd src/main/java
javac com/samegame/*.java

# Run the game
java com.samegame.SameGameGUI

# Run tests (with assertions)
java -ea com.samegame.TestGraph
```

## Game Controls

- **Mouse Hover**: Highlight clusters of same-colored tiles
- **Left Click**: Remove a cluster (minimum 2 tiles)
- **R Key**: Restart game with new random grid
- **ESC Key**: Quit game

## Key Conversion Details

### Language Mappings

| C++ | Java |
|-----|------|
| `vector<T>` | `ArrayList<T>` / `List<T>` |
| `unordered_map<K,V>` | `HashMap<K,V>` |
| `unordered_set<T>` | `HashSet<T>` |
| `queue<T>` | `Queue<T>` / `LinkedList<T>` |
| `pair<T1,T2>` | Custom `Pair<F,S>` class |
| `tuple<T1,T2,T3,T4>` | Custom `ClusterInfo` class |
| Pointers (`*`) | Object references |
| Header files (`.h`) | Single `.java` files |

### GUI Framework Mappings

| SDL2 (C++) | Java Swing |
|------------|------------|
| `SDL_Window` | `JFrame` |
| `SDL_Renderer` | `Graphics2D` |
| `SDL_Color` | `java.awt.Color` |
| `SDL_Rect` | `java.awt.Rectangle` |
| `SDL_Event` | AWT Event Listeners |
| `TTF_Font` | `java.awt.Font` |
| `SDL_RenderFillRect()` | `fillRect()` |
| `SDL_RenderDrawRect()` | `drawRect()` |
| Manual game loop | `javax.swing.Timer` |

## Project Structure

```
src/main/java/com/samegame/
├── Node.java              # Graph node structure (from SameGame.h)
├── SameGame.java          # Game logic (from SameGame.cpp)
├── SameGameGUI.java       # Swing GUI (from main.cpp)
└── TestGraph.java         # Test suite (from test_graph.cpp)
```

## Features Preserved

All features from the C++ version are maintained:

✅ Graph-based tile representation  
✅ BFS cluster detection algorithm  
✅ Vertical and horizontal gravity  
✅ Turn-based gameplay (User vs Computer)  
✅ Greedy AI for computer moves  
✅ Score calculation: `(cluster_size - 2)²`  
✅ Visual tile highlighting  
✅ Real-time score display  
✅ Game over detection  
✅ Restart functionality  

## Advantages of Java Version

1. **No external dependencies**: Swing is built into JDK
2. **Cross-platform**: Runs on Windows, macOS, Linux without recompilation
3. **Memory management**: Automatic garbage collection
4. **Easier distribution**: Single JAR file
5. **IDE support**: Better tooling and debugging
6. **No manual library installation**: SDL2/TTF not needed

## Documentation

- **BUILD_JAVA.md**: Comprehensive build and usage instructions
- **BUILD.md**: Original C++ build instructions (still available)
- **GRAPH_STRUCTURE.md**: Original algorithm documentation (applies to both)

## Testing

Run the comprehensive test suite:

```bash
# With Maven
mvn exec:java -Dexec.mainClass="com.samegame.TestGraph"

# Or directly
cd src/main/java
java -ea com.samegame.TestGraph
```

All tests validate:
- Initialization
- Tile operations
- Cluster detection
- Gravity mechanics
- Turn-based gameplay
- AI functionality

## Building a JAR

Create an executable JAR file:

```bash
mvn clean package
java -jar target/same-game-1.0.0.jar
```

## Comparison with C++ Version

### Similarities:
- Identical game logic and algorithms
- Same scoring system
- Same AI strategy (greedy)
- Same visual appearance
- Same controls and gameplay

### Differences:
- **Language**: Java instead of C++
- **GUI**: Swing instead of SDL2
- **Build**: Maven instead of g++/MinGW
- **Dependencies**: None (vs SDL2, SDL2_ttf)
- **Memory**: GC managed (vs manual)

## Performance

The Java version provides comparable performance to the C++ version for this game's scale (6x8 grid):
- Smooth 60 FPS rendering
- Instant cluster detection
- Real-time user interaction
- Negligible overhead for game logic

## Contributing

When making changes:
1. Ensure tests pass: `java -ea com.samegame.TestGraph`
2. Maintain code structure consistency with C++ version
3. Follow Java naming conventions
4. Document any algorithm changes

## License

This is a direct conversion of the C++ implementation, maintaining the original structure and logic.

## See Also

- Original C++ version (in repository root)
- BUILD_JAVA.md for detailed build instructions
- GRAPH_STRUCTURE.md for algorithm details
