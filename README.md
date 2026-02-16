# Same Game

A C++ implementation of the classic "Same Game" puzzle game with a graphical SDL2 interface and an intelligent computer opponent using advanced algorithms.

## Table of Contents
- [Overview](#overview)
- [Game Rules](#game-rules)
- [Architecture](#architecture)
- [Key Features](#key-features)
- [Data Structures](#data-structures)
- [Algorithms](#algorithms)
- [File Structure](#file-structure)
- [Building and Running](#building-and-running)
- [Controls](#controls)
- [Technical Details](#technical-details)

## Overview

Same Game is a tile-matching puzzle game where the objective is to remove groups of same-colored tiles from a grid. This implementation features:

- **Graphical User Interface**: Built with SDL2 and SDL2_ttf
- **Two-Player Mode**: User vs Computer with turn-based gameplay
- **Intelligent AI**: Computer opponent using Divide & Conquer, Dynamic Programming, and Minimax algorithms
- **Graph-Based Architecture**: Modern graph data structure for efficient tile management

## Game Rules

1. **Cluster Selection**: Click on a group of 2 or more adjacent tiles of the same color
2. **Scoring**: Points are awarded based on cluster size: `(size - 2)²`
   - 2 tiles = 0 points
   - 3 tiles = 1 point
   - 4 tiles = 4 points
   - 5 tiles = 9 points
   - And so on...
3. **Gravity**: After removing a cluster:
   - Tiles fall down to fill empty spaces (vertical gravity)
   - Empty columns shift left (horizontal gravity)
4. **Turn-Based**: Players alternate turns (User starts first)
5. **Game End**: Game ends when no more valid moves exist (no clusters of 2+ tiles)
6. **Winner**: Player with the highest score wins

## Architecture

### High-Level Design

```
┌─────────────────────────────────────────────────────────┐
│                     main.cpp                            │
│  ┌──────────────────────────────────────────────────┐   │
│  │           SameGameGUI (SDL2 Interface)           │   │
│  │  - Window & Rendering                            │   │
│  │  - User Input Handling                           │   │
│  │  - Visual Feedback                               │   │
│  └──────────────────┬───────────────────────────────┘   │
│                     │                                   │
└─────────────────────┼───────────────────────────────────┘
                      │
                      ▼
┌─────────────────────────────────────────────────────────┐
│              SameGame.h / SameGame.cpp                  │
│  ┌──────────────────────────────────────────────────┐   │
│  │          Core Game Logic (SameGame Class)        │   │
│  │                                                  │   │
│  │  Graph Structure:                               │   │
│  │  - vector<Node> nodes                           │   │
│  │  - vector<vector<int>> nodeGrid                 │   │
│  │                                                  │   │
│  │  Game State:                                     │   │
│  │  - Score tracking (user vs computer)            │   │
│  │  - Turn management                              │   │
│  │  - Move validation                              │   │
│  │                                                  │   │
│  │  Algorithms:                                     │   │
│  │  - BFS for cluster detection                    │   │
│  │  - Gravity simulation                           │   │
│  │  - AI decision making (D&C + DP + Minimax)      │   │
│  └──────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────┘
```

## Key Features

### 1. **Graph-Based Data Structure**
The game uses a graph representation instead of a traditional 2D array, providing:
- More intuitive tile connectivity
- Efficient neighbor queries through adjacency lists
- Flexibility for future non-rectangular grids
- Clear separation of position, color, and connectivity

### 2. **Intelligent AI Opponent**
The computer opponent uses a combination of algorithms:

- **Divide & Conquer**: Efficiently finds all clusters by recursively splitting the column range
- **Dynamic Programming**: Memoizes board states to avoid recalculating positions
- **Minimax with Negamax**: Looks ahead 3 moves to choose optimal plays
- **Pruning**: Only evaluates top 6 largest clusters to improve performance

### 3. **Real-Time Visual Feedback**
- Hover highlighting shows potential clusters before clicking
- Cluster size and point preview
- Score tracking for both players
- Turn indicator
- Game over detection with winner announcement

### 4. **Comprehensive Testing**
Includes a test suite (`test_graph.cpp`) that validates:
- Grid initialization
- Tile queries and state management
- Cluster detection algorithm
- Gravity mechanics
- Score calculation
- Turn-based gameplay

## Data Structures

### Node Structure
```cpp
struct Node {
    int row;              // Current row position
    int col;              // Current column position
    char color;           // Tile color ('G', 'W', 'R', 'B', 'Y')
    bool active;          // Whether tile is still in play
    vector<int> neighbors; // Indices of adjacent nodes
};
```

### SameGame Class
```cpp
class SameGame {
private:
    vector<Node> nodes;                // All tiles as graph nodes
    vector<vector<int>> nodeGrid;      // Position → node index mapping
    int rows, cols;                    // Grid dimensions
    int score, moves;                  // Game state
    bool isUserTurn;                   // Turn management
    int userScore, computerScore;      // Player scores
    unordered_map<string, int> dpMemo; // DP memoization cache
    
public:
    // Game state queries
    char getTile(int row, int col);
    bool isTileActive(int row, int col);
    int getClusterSize(int row, int col);
    
    // Game actions
    bool removeCluster(int row, int col);
    bool hasMovesLeft();
    
    // AI decision making
    pair<int, int> getBestMove();
};
```

## Algorithms

### 1. Cluster Detection (BFS)
```
Time Complexity: O(N) where N = number of tiles in cluster
Space Complexity: O(N)

Algorithm:
1. Start from a tile position
2. Use queue to explore same-colored neighbors
3. Mark visited tiles to avoid duplicates
4. Return all tiles in connected component
```

### 2. Gravity Application
```
Time Complexity: O(rows × cols)

Vertical Gravity:
- For each column, move active tiles down
- Fill from bottom to top

Horizontal Gravity:
- Shift non-empty columns to the left
- Compact the grid horizontally
```

### 3. Computer AI (Minimax with DP)
```
Time Complexity: O(B^D) where B = branching factor, D = depth
Space Complexity: O(S) where S = number of unique board states

Algorithm:
1. Get all available clusters
2. Sort by size (largest first)
3. For top N clusters:
   a. Simulate the move
   b. Recursively evaluate with lookahead
   c. Use memoization to cache results
   d. Restore board state
4. Choose move with best evaluation score
```

### 4. Board State Memoization
```cpp
string boardStateKey() {
    // Creates unique string representation:
    // Turn indicator + grid colors
    // Example: "UGGWRWWWRWWWR" for user turn
    string key = (isUserTurn ? 'U' : 'C');
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            key += active ? color : '.';
        }
    }
    return key;
}
```

## File Structure

```
Same-Game/
├── SameGame.h              # Game logic class header
│   └── Defines Node structure and SameGame class interface
│
├── SameGame.cpp            # Game logic implementation
│   ├── Graph construction and management
│   ├── Cluster detection (BFS)
│   ├── Gravity simulation
│   ├── AI algorithms (D&C, DP, Minimax)
│   └── Game state management
│
├── main.cpp                # SDL2 GUI implementation
│   ├── SameGameGUI class
│   ├── Rendering and visual feedback
│   ├── User input handling
│   └── Game loop
│
├── test_graph.cpp          # Test suite
│   └── Validates graph implementation
│
├── greedy.cpp              # Original console version (deprecated)
│   └── Simple greedy algorithm implementation
│
├── BUILD.md                # Build instructions and prerequisites
├── GRAPH_STRUCTURE.md      # Detailed graph implementation docs
├── build.sh / build.bat    # Build scripts
└── README.md               # This file (codebase documentation)
```

## Building and Running

### Prerequisites
- C++17 compiler (g++ or MinGW)
- SDL2 library
- SDL2_ttf library

### Installation (Windows/MSYS2)
```bash
# Install MSYS2 from https://www.msys2.org/
# Open MSYS2 MinGW 64-bit terminal
pacman -S mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_ttf
```

### Build Commands
```bash
# Build the game
g++ -std=c++17 -I. SameGame.cpp main.cpp -o SameGame.exe \
    -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf

# Run the game
./SameGame.exe

# Build tests
g++ -std=c++17 -I. SameGame.cpp test_graph.cpp -o test_graph

# Run tests
./test_graph
```

### Using Build Scripts
```bash
# Linux/Mac
./build.sh

# Windows
build.bat
```

## Controls

| Input | Action |
|-------|--------|
| **Mouse Hover** | Highlight cluster under cursor |
| **Left Click** | Remove highlighted cluster (on user's turn) |
| **R Key** | Restart game with new random grid |
| **ESC Key** | Quit game |

## Technical Details

### Grid Configuration
- Default size: 6 rows × 8 columns
- 5 colors: Green (G), White (W), Red (R), Blue (Y), Yellow (Y)
- Random initialization using time-based seed

### Display Configuration
```cpp
const int WINDOW_WIDTH = 750;
const int WINDOW_HEIGHT = 650;
const int TILE_SIZE = 80;
const int GRID_OFFSET_X = 80;
const int GRID_OFFSET_Y = 140;
```

### AI Configuration
```cpp
const Uint32 COMPUTER_MOVE_DELAY = 3000; // 3 seconds between moves
const int LOOK_AHEAD_DEPTH = 3;          // Minimax depth
const int TOP_N_MOVES = 6;               // Evaluate top 6 clusters
```

### Scoring Formula
```cpp
points = (clusterSize - 2)²

Examples:
- 2 tiles:  (2-2)² = 0 points
- 3 tiles:  (3-2)² = 1 point
- 4 tiles:  (4-2)² = 4 points
- 5 tiles:  (5-2)² = 9 points
- 10 tiles: (10-2)² = 64 points
```

### Color Mapping
```cpp
'G' → Green  (0, 200, 0)
'W' → White  (255, 255, 255)
'R' → Red    (200, 0, 0)
'B' → Blue   (0, 0, 200)
'Y' → Yellow (200, 200, 0)
```

## Design Decisions

### Why Graph-Based Structure?
The transition from a 2D array to a graph provides:

1. **Better Abstraction**: Tiles are explicitly connected entities
2. **Efficient Queries**: O(1) neighbor access through adjacency lists
3. **Future-Proof**: Can support hexagonal or irregular grids
4. **Algorithm-Friendly**: Graph algorithms (BFS, DFS) apply naturally

### Why Minimax for AI?
1. **Optimal Play**: Looks ahead to choose best moves
2. **Competitive**: Creates challenging opponent
3. **Balanced**: Not too easy, not unbeatable
4. **Demonstrative**: Shows AI game theory concepts

### Why Turn-Based?
1. **Strategic Depth**: Players must think ahead
2. **Fair Competition**: Both players have equal opportunities
3. **Learning Tool**: Users can observe AI's strategies
4. **Game Flow**: Creates natural pause for observation

## Performance Characteristics

### Time Complexities
- **Cluster Detection**: O(C) where C = cluster size
- **Gravity Application**: O(R × C) where R×C = grid size
- **Move Validation**: O(N) where N = total tiles
- **AI Move Selection**: O(B^D) where B = branching factor, D = depth
- **Board State Hash**: O(R × C)

### Space Complexities
- **Graph Storage**: O(N) for N tiles
- **Adjacency Lists**: O(4N) average (4 neighbors per tile)
- **DP Memoization**: O(S) where S = unique board states
- **BFS Queue**: O(C) for cluster detection

## Testing

The test suite validates:
- ✓ Grid initialization
- ✓ Tile queries (getTile, isTileActive)
- ✓ Cluster detection with various sizes
- ✓ Cluster removal and scoring
- ✓ Gravity mechanics (vertical and horizontal)
- ✓ getAllClusters functionality
- ✓ Game state (hasMovesLeft, reset)
- ✓ Turn-based gameplay
- ✓ Edge cases (invalid positions, single tiles)

Run tests with:
```bash
./test_graph
```

Expected output: All tests should pass with "✓" marks.

## Future Enhancements

Potential improvements for the codebase:
1. **Difficulty Levels**: Adjustable AI depth and evaluation
2. **Undo/Redo**: Move history and backtracking
3. **Save/Load**: Game state persistence
4. **Statistics**: Track high scores and win rates
5. **Animations**: Smooth tile removal and gravity effects
6. **Sound Effects**: Audio feedback for moves
7. **Online Multiplayer**: Network play support
8. **Different Grid Sizes**: Configurable dimensions
9. **Power-ups**: Special tiles or abilities
10. **Campaign Mode**: Progressive difficulty levels

## Contributing

When contributing to this codebase:
1. Maintain the graph-based architecture
2. Add tests for new features
3. Update documentation
4. Follow existing code style
5. Ensure backward compatibility

## License

This is an educational project demonstrating:
- Graph data structures
- Game AI algorithms (Minimax, Dynamic Programming)
- SDL2 GUI programming
- Algorithm optimization techniques

## Authors

- **codervaruns** - Initial implementation and architecture

## Acknowledgments

- Classic Same Game for inspiration
- SDL2 library for graphics
- Graph theory and AI algorithms from computer science fundamentals
