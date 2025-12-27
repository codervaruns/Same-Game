# C++ to Java Conversion Reference

This document provides a detailed mapping of how the C++ Same Game project was converted to Java.

## File Mappings

| C++ File | Java File | Purpose |
|----------|-----------|---------|
| `SameGame.h` | `Node.java` + `SameGame.java` | Header split into separate class files |
| `SameGame.cpp` | `SameGame.java` | Game logic implementation |
| `main.cpp` | `SameGameGUI.java` | GUI implementation (SDL2 → Swing) |
| `test_graph.cpp` | `TestGraph.java` | Test suite |
| `greedy.cpp` | *(Integrated in SameGame.java)* | AI logic integrated into main class |
| `build.bat` / `build.sh` | `run_java.bat` / `run_java.sh` | Build/run scripts |
| *(none)* | `pom.xml` | Maven build configuration (new) |

## Class/Struct Mappings

### Node Structure

**C++ (SameGame.h):**
```cpp
struct Node {
    int row;
    int col;
    char color;
    bool active;
    vector<int> neighbors;
    
    Node(int r, int c, char clr);
};
```

**Java (Node.java):**
```java
public class Node {
    public int row;
    public int col;
    public char color;
    public boolean active;
    public List<Integer> neighbors;
    
    public Node(int r, int c, char clr);
}
```

### SameGame Class

**C++ (SameGame.h):**
```cpp
class SameGame {
private:
    vector<Node> nodes;
    unordered_map<int, unordered_map<int, int>> posToNodeIndex;
    int rows, cols, score, moves;
    bool isUserTurn;
    int userScore, computerScore;
    
public:
    SameGame(const vector<vector<char>>& initialGrid);
    // ... methods
};
```

**Java (SameGame.java):**
```java
public class SameGame {
    private List<Node> nodes;
    private Map<Integer, Map<Integer, Integer>> posToNodeIndex;
    private int rows, cols, score, moves;
    private boolean isUserTurn;
    private int userScore, computerScore;
    
    public SameGame(char[][] initialGrid) { }
    // ... methods
}
```

## Data Structure Mappings

| C++ Type | Java Type | Notes |
|----------|-----------|-------|
| `vector<T>` | `ArrayList<T>` or `List<T>` | Dynamic array |
| `unordered_map<K,V>` | `HashMap<K,V>` or `Map<K,V>` | Hash table |
| `unordered_set<T>` | `HashSet<T>` or `Set<T>` | Hash set |
| `queue<T>` | `Queue<T>` or `LinkedList<T>` | FIFO queue |
| `pair<T1,T2>` | Custom `Pair<F,S>` class | Key-value pair |
| `tuple<T1,T2,T3,T4>` | Custom `ClusterInfo` class | Multiple values |
| `bool` | `boolean` | Boolean type |
| `char` | `char` | Character type |
| `int` | `int` | Integer type |
| Raw pointers (`*`) | Object references | Memory management |
| `const` reference (`const T&`) | Regular reference | Immutability |
| `nullptr` | `null` | Null pointer/reference |

## Algorithm Mappings

### BFS Cluster Detection

**C++ (SameGame.cpp):**
```cpp
vector<pair<int, int>> SameGame::detectClusterBFS(int startRow, int startCol) {
    vector<pair<int, int>> cluster;
    unordered_set<int> visited;
    queue<int> q;
    
    q.push(startNodeIdx);
    visited.insert(startNodeIdx);
    
    while (!q.empty()) {
        int currentIdx = q.front();
        q.pop();
        // ... BFS logic
    }
    return cluster;
}
```

**Java (SameGame.java):**
```java
private List<Pair<Integer, Integer>> detectClusterBFS(int startRow, int startCol) {
    List<Pair<Integer, Integer>> cluster = new ArrayList<>();
    Set<Integer> visited = new HashSet<>();
    Queue<Integer> q = new LinkedList<>();
    
    q.add(startNodeIdx);
    visited.add(startNodeIdx);
    
    while (!q.isEmpty()) {
        int currentIdx = q.poll();
        // ... BFS logic
    }
    return cluster;
}
```

### Gravity System

Both implementations use identical two-phase gravity:

1. **Vertical Gravity** - Tiles fall down
2. **Horizontal Gravity** - Empty columns shift left

The logic is nearly identical, with only syntax differences.

## GUI Framework Mappings

### Window Creation

**C++ (main.cpp - SDL2):**
```cpp
SDL_Init(SDL_INIT_VIDEO);
TTF_Init();
window = SDL_CreateWindow("Same Game", 
                          SDL_WINDOWPOS_CENTERED, 
                          SDL_WINDOWPOS_CENTERED,
                          WINDOW_WIDTH, WINDOW_HEIGHT, 
                          SDL_WINDOW_SHOWN);
renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
```

**Java (SameGameGUI.java - Swing):**
```java
setTitle("Same Game");
setSize(WINDOW_WIDTH, WINDOW_HEIGHT);
setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
setResizable(false);
setVisible(true);
```

### Rendering

**C++ (main.cpp - SDL2):**
```cpp
void drawTile(int row, int col, bool highlight) {
    SDL_Color color = getColorForTile(game->getTile(row, col));
    SDL_Rect tileRect = {x + 2, y + 2, TILE_SIZE - 4, TILE_SIZE - 4};
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &tileRect);
}
```

**Java (SameGameGUI.java - Swing):**
```java
private void drawTile(Graphics2D g, int row, int col, boolean highlight) {
    Color color = getColorForTile(game.getTile(row, col));
    g.setColor(color);
    g.fillRect(x + 2, y + 2, TILE_SIZE - 4, TILE_SIZE - 4);
}
```

### Event Handling

**C++ (main.cpp - SDL2):**
```cpp
SDL_Event e;
while (SDL_PollEvent(&e) != 0) {
    if (e.type == SDL_QUIT) {
        quit = true;
    } else if (e.type == SDL_MOUSEMOTION) {
        handleMouseMove(e.motion.x, e.motion.y);
    } else if (e.type == SDL_MOUSEBUTTONDOWN) {
        handleMouseClick(e.button.x, e.button.y);
    }
}
```

**Java (SameGameGUI.java - Swing):**
```java
addMouseMotionListener(new MouseMotionAdapter() {
    @Override
    public void mouseMoved(MouseEvent e) {
        handleMouseMove(e.getX(), e.getY());
    }
});

addMouseListener(new MouseAdapter() {
    @Override
    public void mouseClicked(MouseEvent e) {
        handleMouseClick(e.getX(), e.getY());
    }
});
```

### Game Loop

**C++ (main.cpp - SDL2):**
```cpp
void run() {
    bool quit = false;
    while (!quit) {
        // Event handling
        while (SDL_PollEvent(&e) != 0) { ... }
        
        // Computer move logic
        if (!gameOver && !game->getUserTurn()) { ... }
        
        // Render
        render();
        SDL_Delay(16); // ~60 FPS
    }
}
```

**Java (SameGameGUI.java - Swing):**
```java
Timer gameTimer = new Timer(16, e -> {
    // Computer move logic
    if (!gameOver && !game.getUserTurn()) { ... }
    
    // Repaint triggers paintComponent()
    gamePanel.repaint();
});
gameTimer.start();
```

### Color Definitions

**C++ (main.cpp - SDL2):**
```cpp
SDL_Color getColorForTile(char tile) {
    switch(tile) {
        case 'G': return {0, 200, 0, 255};      // Green
        case 'W': return {255, 255, 255, 255};  // White
        // ...
    }
}
```

**Java (SameGameGUI.java - Swing):**
```java
private Color getColorForTile(char tile) {
    switch(tile) {
        case 'G': return new Color(0, 200, 0);      // Green
        case 'W': return new Color(255, 255, 255);  // White
        // ...
    }
}
```

## Build System Mappings

### C++ Build

**Using g++/MinGW:**
```bash
g++ -std=c++17 -I. SameGame.cpp main.cpp -o SameGame.exe \
    -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf
```

**Dependencies:**
- SDL2 library
- SDL2_ttf library
- MinGW/g++ compiler

### Java Build

**Using Maven:**
```bash
mvn clean compile
mvn exec:java -Dexec.mainClass="com.samegame.SameGameGUI"
```

**Using javac:**
```bash
cd src/main/java
javac com/samegame/*.java
java com.samegame.SameGameGUI
```

**Dependencies:**
- None! (Swing is part of JDK)

## Testing

### C++ Test Suite

**File:** `test_graph.cpp`

```cpp
#include "SameGame.h"
#include <iostream>
#include <cassert>

int main() {
    // Tests using assert()
    assert(game.getRows() == 3);
    // ...
    cout << "All tests passed! ✓" << endl;
    return 0;
}
```

**Run:**
```bash
g++ -std=c++17 SameGame.cpp test_graph.cpp -o test_graph
./test_graph
```

### Java Test Suite

**File:** `TestGraph.java`

```java
package com.samegame;

public class TestGraph {
    public static void main(String[] args) {
        // Tests using assert
        assert game.getRows() == 3;
        // ...
        System.out.println("All tests passed! ✓");
    }
}
```

**Run:**
```bash
cd src/main/java
javac com/samegame/*.java
java -ea com.samegame.TestGraph  # -ea enables assertions
```

## Key Differences

### Memory Management

**C++:**
- Manual memory management with pointers
- Need to explicitly manage SDL resources
- `delete`, destructors, RAII

**Java:**
- Automatic garbage collection
- No manual memory management
- Finalize methods (rarely needed)

### Header Files

**C++:**
- Separate `.h` header and `.cpp` implementation files
- Need include guards or `#pragma once`
- Forward declarations

**Java:**
- Single `.java` file per class
- No header files
- Automatic class discovery

### Packages and Namespaces

**C++:**
```cpp
using namespace std;
// Global scope or namespace blocks
```

**Java:**
```java
package com.samegame;
// All classes in a package
```

### Access Modifiers

**C++:**
```cpp
class SameGame {
private:
    int rows;
public:
    int getRows() const;
};
```

**Java:**
```java
public class SameGame {
    private int rows;
    public int getRows() { return rows; }
}
```

### Const Correctness

**C++:**
```cpp
int getRows() const;  // Method doesn't modify object
const vector<char>& getColors() const;  // Return const reference
```

**Java:**
```java
public int getRows() { }  // No const keyword
public List<Character> getColors() { }  // Returns reference (can't enforce const)
```

## Performance Considerations

### C++ Advantages:
- Direct hardware access via SDL2
- No garbage collection overhead
- Better control over memory layout
- Potentially faster for very large grids

### Java Advantages:
- JIT compilation optimizes hot paths
- Modern GC is very efficient
- Swing double-buffering is optimized
- Negligible difference for this game's scale

## Portability

### C++:
- Requires recompilation for each platform
- Need to install SDL2 libraries on each platform
- Different build commands for Windows/Linux/Mac

### Java:
- Write once, run anywhere
- No external dependencies (Swing is built-in)
- Same JAR file runs on all platforms with JVM

## Conclusion

The Java conversion maintains 100% feature parity with the C++ version while providing:
- ✅ Simpler build process
- ✅ No external dependencies
- ✅ Better cross-platform support
- ✅ Automatic memory management
- ✅ Easier distribution (single JAR file)

Both versions use identical algorithms and provide the same gameplay experience.
