# Graph Data Structure Implementation

## Overview
This document describes the conversion of the Same Game from a 2D array grid data structure to a graph-based data structure.

## Data Structure Changes

### Previous Implementation (2D Array)
```cpp
vector<vector<char>> grid;           // Stored tile colors
vector<vector<bool>> active;         // Stored tile active status
```

### New Implementation (Graph)
```cpp
struct Node {
    int row;
    int col;
    char color;
    bool active;
    vector<int> neighbors;  // indices of adjacent nodes
};

vector<Node> nodes;  // Graph nodes
unordered_map<int, unordered_map<int, int>> posToNodeIndex;  // Position mapping
```

## Key Features

### 1. Node Structure
Each tile is represented as a node in the graph containing:
- **Position**: (row, col) coordinates
- **Color**: The tile's color character
- **Active**: Whether the tile is active or removed
- **Neighbors**: List of adjacent node indices (up, right, down, left)

### 2. Graph Construction
The graph is built during initialization:
1. Create a node for each tile in the initial grid
2. Build position-to-index mapping for efficient lookups
3. Establish neighbor relationships between adjacent nodes

### 3. Cluster Detection
BFS traversal now uses the graph structure:
- Start from a node and traverse using neighbor relationships
- No need to check bounds or directions manually
- More efficient neighbor lookups

### 4. Gravity Implementation
Gravity is applied by updating node positions:
1. **Vertical gravity**: Move nodes downward by updating row positions
2. **Horizontal gravity**: Shift columns left by updating col positions
3. **Update neighbors**: Rebuild adjacency relationships after position changes

## Benefits of Graph Structure

1. **More intuitive representation**: Tiles are explicitly connected as nodes
2. **Flexible topology**: Can support non-rectangular grids in the future
3. **Efficient neighbor queries**: Direct access through neighbor list
4. **Clear separation of concerns**: Position, color, and connectivity are distinct
5. **Better for algorithms**: Graph algorithms naturally apply (BFS, DFS, etc.)

## Testing

A comprehensive test suite (`test_graph.cpp`) validates:
- Basic initialization and grid setup
- getTile() and isTileActive() queries
- Cluster detection with BFS
- Cluster removal and gravity
- Score calculation
- User vs Computer mode
- Edge cases (single tiles, invalid positions)

All tests pass successfully, confirming the graph implementation maintains all original functionality.

## Usage

Compile the test:
```bash
g++ -std=c++17 -I. SameGame.cpp test_graph.cpp -o test_graph
```

Run the test:
```bash
./test_graph
```

## Backward Compatibility

The public API remains unchanged:
- `getTile(row, col)` - Get tile color at position
- `isTileActive(row, col)` - Check if tile is active
- `getCluster(row, col)` - Get cluster at position
- `removeCluster(row, col)` - Remove cluster at position
- `hasMovesLeft()` - Check if moves are available

All existing code using the SameGame class continues to work without modifications.
