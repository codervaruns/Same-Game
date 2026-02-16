# Codebase Summary - Same Game

## Quick Overview
This is a C++ implementation of the classic Same Game puzzle with:
- **SDL2 GUI** for graphics
- **User vs Computer** gameplay
- **Graph-based** architecture
- **AI opponent** using Minimax + Dynamic Programming

## Project Purpose
Educational game demonstrating:
1. Graph data structures in practice
2. Game AI algorithms (Minimax, DP, D&C)
3. SDL2 GUI development
4. Algorithm optimization techniques

## Key Components

### 1. Core Game Engine (`SameGame.h/cpp`)
**What it does**: Manages all game logic and state
- Graph-based tile representation
- Cluster detection using BFS
- Gravity simulation (vertical + horizontal)
- AI decision making with lookahead
- Turn-based gameplay management

**Key Algorithm**: Minimax with DP memoization
```
Look ahead 3 moves → Evaluate board states → Choose optimal move
```

### 2. GUI Layer (`main.cpp`)
**What it does**: Visual interface and user interaction
- SDL2 window and rendering
- Mouse hover preview with highlighting
- Click handling for moves
- Score and turn display
- Computer move animation (3 second delay)

**Main Loop**: Event handling → State updates → Rendering (60 FPS)

### 3. Graph Structure
**Why graphs instead of 2D arrays?**
- Explicit tile connectivity
- O(1) neighbor access via adjacency lists
- Future-proof for non-rectangular grids
- Natural fit for BFS/DFS algorithms

**Structure**:
```cpp
Node {
  row, col      // Position
  color         // G/W/R/B/Y
  active        // Still in play?
  neighbors[]   // Adjacent node indices
}
```

## How It Works

### Game Flow
```
1. Initialize 6×8 random grid
2. User's turn: Click cluster → Remove → Gravity → Switch turn
3. Computer's turn: AI evaluates → Best move → Remove → Gravity → Switch turn
4. Repeat until no moves left
5. Highest score wins
```

### AI Decision Process
```
1. Find all clusters (getAllClusters)
2. Sort by size (larger = better)
3. For top 6 clusters:
   - Simulate move
   - Recursively evaluate (3 moves deep)
   - Use memoization for speed
   - Undo simulation
4. Choose cluster with best score differential
```

### Gravity Mechanics
```
Vertical: For each column, active tiles fall down
Horizontal: Empty columns shift left
Result: Compact grid with no gaps
```

## File Responsibilities

| File | Purpose | Lines | Key Features |
|------|---------|-------|--------------|
| `SameGame.h` | Class interface | 96 | Node struct, SameGame class definition |
| `SameGame.cpp` | Game logic | 455 | BFS, gravity, AI algorithms, DP |
| `main.cpp` | GUI & rendering | 402 | SDL2 interface, user input, visuals |
| `test_graph.cpp` | Testing | 150+ | Validates all core functionality |
| `greedy.cpp` | Legacy | 157 | Original console version (deprecated) |

## Code Statistics

- **Total Lines**: ~1,100 (excluding tests)
- **Classes**: 2 (SameGame, SameGameGUI)
- **Algorithms**: 5+ (BFS, Minimax, DP, D&C, Gravity)
- **Data Structures**: Graph (nodes + adjacency lists)
- **Dependencies**: SDL2, SDL2_ttf

## Architecture Patterns

### 1. **Separation of Concerns**
- Game logic (SameGame) ↔ Presentation (SameGameGUI)
- No GUI code in game logic
- Game logic testable independently

### 2. **Graph Representation**
- Nodes store state (position, color, active)
- Adjacency lists track connections
- Position mapping for O(1) lookups

### 3. **State Management**
- Snapshot/restore for AI simulation
- Turn-based state tracking
- Score tracking per player

### 4. **Algorithm Composition**
- D&C for cluster finding
- BFS for traversal
- DP for memoization
- Minimax for decision making

## Performance Notes

### Optimizations
✓ DP memoization prevents recalculation
✓ Top-N pruning limits search space
✓ Direct neighbor access via graph
✓ Board state hashing for quick lookup

### Bottlenecks
⚠ AI evaluation depth limited to 3 moves
⚠ Minimax explores 6^3 = 216 states max
⚠ String-based board hashing (could use zobrist)

## Testing Coverage

Validated scenarios:
- ✓ Grid initialization
- ✓ Tile queries and state
- ✓ Cluster detection (various sizes)
- ✓ Gravity (vertical + horizontal)
- ✓ Scoring calculation
- ✓ Turn management
- ✓ Game ending conditions
- ✓ Edge cases

Run: `./test_graph`

## Common Operations

### Add a New Color
1. Update `generateRandomGrid()` colors array
2. Add color case in `getColorForTile()`
3. Update documentation

### Adjust AI Difficulty
```cpp
int lookDepth = 3;        // Increase for harder
int limit = 6;            // Evaluate more moves
```

### Change Grid Size
```cpp
generateRandomGrid(rows, cols);  // In main()
```

### Modify Scoring
```cpp
int points = (clusterSize - 2) * (clusterSize - 2);  // In removeCluster()
```

## Dependencies

### Required Libraries
- **SDL2**: Window, rendering, events
- **SDL2_ttf**: Text rendering
- **C++17**: Modern C++ features

### Installation (MSYS2)
```bash
pacman -S mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_ttf
```

## Build & Run

```bash
# Build game
g++ -std=c++17 -I. SameGame.cpp main.cpp -o SameGame.exe \
    -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf

# Run
./SameGame.exe

# Build tests
g++ -std=c++17 -I. SameGame.cpp test_graph.cpp -o test_graph

# Run tests
./test_graph
```

## Design Philosophy

### Why This Architecture?
1. **Educational**: Demonstrates multiple CS concepts
2. **Maintainable**: Clear separation of concerns
3. **Extensible**: Easy to add features
4. **Testable**: Logic isolated from UI
5. **Performant**: Optimized algorithms

### Trade-offs Made
- **Simplicity vs Performance**: Chose readable code over micro-optimizations
- **Features vs Complexity**: Core gameplay over bells and whistles
- **AI Strength vs Speed**: Limited depth for responsive play
- **Portability vs Features**: Basic SDL2 for cross-platform support

## Learning Outcomes

By studying this codebase, you'll learn:
1. **Graph data structures** in practical application
2. **Game AI** using Minimax and Dynamic Programming
3. **SDL2** for game development
4. **Algorithm design** and optimization
5. **Software architecture** patterns
6. **C++17** modern features

## Next Steps for Contributors

### Easy Tasks
- Add sound effects
- Implement undo/redo
- Add high score persistence
- Create difficulty settings
- Add more color schemes

### Medium Tasks
- Smooth animations
- Online multiplayer
- Save/load game state
- Campaign mode
- Power-ups system

### Advanced Tasks
- Alpha-beta pruning for AI
- Neural network player
- 3D graphics
- Mobile port
- Competitive ranking system

## Resources

- **Full Documentation**: See `README.md`
- **Build Instructions**: See `BUILD.md`
- **Graph Details**: See `GRAPH_STRUCTURE.md`
- **Tests**: Run `test_graph` executable

## Contact

**Repository**: codervaruns/Same-Game
**Purpose**: Educational game development project
**Status**: Fully functional with comprehensive documentation
