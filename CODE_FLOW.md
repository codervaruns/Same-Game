# Code Flow Visualization - Same Game

This document provides visual representations of how the code flows through the Same Game application.

## Table of Contents
- [Application Startup](#application-startup)
- [Game Loop](#game-loop)
- [User Move Flow](#user-move-flow)
- [Computer Move Flow](#computer-move-flow)
- [Cluster Removal Process](#cluster-removal-process)
- [Gravity Application](#gravity-application)
- [AI Decision Tree](#ai-decision-tree)
- [Data Flow](#data-flow)
- [Class Relationships](#class-relationships)

## Application Startup

```
main()
  │
  ├─► generateRandomGrid(6, 8)
  │   └─► Returns: 6×8 grid with random colors (G/W/R/B/Y)
  │
  ├─► SameGame game(initialGrid)
  │   └─► Constructor
  │       └─► reset(initialGrid)
  │           ├─► buildGraph(initialGrid)
  │           │   ├─► Create Node for each tile
  │           │   ├─► Build nodeGrid mapping
  │           │   └─► updateNeighbors()
  │           │       └─► Set adjacency lists
  │           └─► Initialize scores, turns, moves
  │
  ├─► SameGameGUI gui(&game)
  │   └─► Constructor (initialize SDL state)
  │
  ├─► gui.init()
  │   ├─► SDL_Init(SDL_INIT_VIDEO)
  │   ├─► TTF_Init()
  │   ├─► SDL_CreateWindow()
  │   ├─► SDL_CreateRenderer()
  │   └─► TTF_OpenFont()
  │
  └─► gui.run()
      └─► Enter main game loop...
```

## Game Loop

```
gui.run() - Main Game Loop (60 FPS)
  │
  └─► while (!quit)
      │
      ├─► SDL_PollEvent(&e) ───────────── Poll for events
      │   │
      │   ├─► SDL_QUIT ──────────────────► quit = true
      │   │
      │   ├─► SDL_MOUSEMOTION ───────────► handleMouseMove()
      │   │   └─► screenToGrid() → getCluster()
      │   │
      │   ├─► SDL_MOUSEBUTTONDOWN ───────► handleMouseClick()
      │   │   └─► removeCluster() → check game over
      │   │
      │   └─► SDL_KEYDOWN
      │       ├─► R key ─────────────────► handleRestart()
      │       └─► ESC key ───────────────► quit = true
      │
      ├─► Computer Turn Logic ────────────── If !gameOver && !isUserTurn
      │   └─► if (time >= 3 seconds)
      │       └─► executeComputerMove()
      │           └─► getBestMove() → removeCluster()
      │
      ├─► render() ───────────────────────── Render frame
      │   ├─► Clear screen
      │   ├─► drawGrid()
      │   ├─► drawUI()
      │   └─► SDL_RenderPresent()
      │
      └─► SDL_Delay(16) ──────────────────── ~60 FPS
```

## User Move Flow

```
User Clicks on Tile (row, col)
  │
  └─► handleMouseClick(mouseX, mouseY)
      │
      ├─► Check: gameOver? → return
      ├─► Check: isUserTurn? → return if not
      │
      ├─► screenToGrid(mouseX, mouseY) ────► (row, col)
      │
      └─► game.removeCluster(row, col)
          │
          ├─► detectClusterBFS(row, col) ──► Get cluster
          │   │
          │   └─► BFS Algorithm
          │       ├─► Start from (row, col)
          │       ├─► Queue neighbors with same color
          │       ├─► Mark visited
          │       └─► Return cluster list
          │
          ├─► Check: cluster.size() >= 2? ──► If not, return false
          │
          ├─► Remove tiles ──────────────────► Mark nodes inactive
          │
          ├─► Calculate score ───────────────► points = (size - 2)²
          │   ├─► score += points
          │   └─► userScore += points
          │
          ├─► moves++ ───────────────────────► Increment move counter
          │
          ├─► switchTurn() ──────────────────► isUserTurn = false
          │
          └─► applyGravity() ────────────────► See gravity flow below
```

## Computer Move Flow

```
Computer's Turn (after 3 second delay)
  │
  └─► executeComputerMove()
      │
      └─► game.getBestMove() ───────────────► Returns (row, col)
          │
          ├─► Clear DP memo
          │
          ├─► getAllClusters() ─────────────► Get all valid clusters
          │   │
          │   └─► For each active node:
          │       └─► detectClusterBFS() → cluster
          │           └─► If size >= 2, add to list
          │
          ├─► Sort clusters (largest first)
          │
          ├─► Evaluate top 6 clusters ──────► AI Decision Making
          │   │
          │   └─► For each cluster:
          │       │
          │       ├─► saveState() ──────────► Snapshot board
          │       │
          │       ├─► removeCluster() ──────► Simulate move
          │       │
          │       ├─► dpEvaluate(depth=3) ──► Recursive lookahead
          │       │   │
          │       │   └─► Minimax Tree Search
          │       │       ├─► Check memo cache
          │       │       ├─► Get all clusters
          │       │       ├─► Sort by size
          │       │       ├─► Try top 3 moves
          │       │       ├─► Recurse (depth - 1)
          │       │       ├─► Min/Max selection
          │       │       └─► Cache result
          │       │
          │       ├─► Compare scores ────────► Track best move
          │       │
          │       └─► restoreState() ────────► Undo simulation
          │
          └─► Return best (row, col)
              │
              └─► removeCluster(row, col) ──► Execute actual move
```

## Cluster Removal Process

```
removeCluster(row, col)
  │
  ├─► detectClusterBFS(row, col)
  │   │
  │   └─► BFS Traversal:
  │       ┌──────────────────────────────────┐
  │       │ Queue: [(row,col)]               │
  │       │ Visited: {}                      │
  │       │ Cluster: []                      │
  │       └──────────────────────────────────┘
  │           │
  │           ├─► Pop (r,c) from queue
  │           ├─► Add (r,c) to cluster
  │           ├─► For each neighbor (up/right/down/left):
  │           │   ├─► Check: same color?
  │           │   ├─► Check: active?
  │           │   ├─► Check: not visited?
  │           │   └─► Add to queue
  │           └─► Repeat until queue empty
  │
  ├─► Validate: cluster.size() >= 2
  │
  ├─► Mark tiles inactive
  │   └─► For each (r,c) in cluster:
  │       └─► nodes[index].active = false
  │
  ├─► Calculate score
  │   └─► points = (size - 2)²
  │
  ├─► Update player score
  │   └─► If user turn: userScore += points
  │       Else: computerScore += points
  │
  ├─► Increment moves
  │
  ├─► Switch turns
  │
  └─► applyGravity()
```

## Gravity Application

```
applyGravity()
  │
  ├─► PHASE 1: Vertical Gravity ────────────► Tiles fall down
  │   │
  │   └─► For each column (j = 0 to cols-1):
  │       │
  │       ├─► pos = rows - 1 (bottom)
  │       │
  │       └─► For each row (i = rows-1 to 0):
  │           │
  │           ├─► Get node at (i, j)
  │           │
  │           └─► If node.active:
  │               ├─► Move node to row 'pos'
  │               ├─► Update nodeGrid[pos][j]
  │               ├─► Update node.row = pos
  │               └─► pos--
  │
  │   Result: Active tiles at bottom, gaps at top
  │
  ├─► PHASE 2: Horizontal Gravity ──────────► Columns shift left
  │   │
  │   ├─► col = 0
  │   │
  │   └─► For each column (j = 0 to cols-1):
  │       │
  │       ├─► Check if column has any active tiles
  │       │
  │       └─► If has tiles:
  │           ├─► Move entire column to position 'col'
  │           ├─► For each row:
  │           │   ├─► Update nodeGrid[i][col]
  │           │   └─► Update node.col = col
  │           └─► col++
  │
  │   Result: No empty columns in the middle
  │
  └─► updateNeighbors() ────────────────────► Rebuild adjacency
      │
      └─► For each active node:
          └─► Check 4 directions (up/right/down/left)
              └─► Add active neighbors to adjacency list
```

## AI Decision Tree

```
getBestMove() - Computer AI Decision Process
  │
  ├─► Get All Clusters
  │   └─► Result: [(size:6,color:W,row:1,col:2), (size:4,color:G,row:0,col:0), ...]
  │
  ├─► Sort by Size (descending)
  │   └─► Result: [(6,W,1,2), (4,G,0,0), (3,R,2,3), (2,B,1,5), ...]
  │
  ├─► Limit to Top 6 Clusters
  │   └─► Pruning to reduce search space
  │
  └─► Evaluate Each Cluster:
      │
      For Cluster #1 (size 6):
      │
      ├─► Save Current State
      │
      ├─► Simulate Move
      │   └─► Remove cluster → Apply gravity
      │
      ├─► Evaluate Future (depth = 3)
      │   │
      │   └─► dpEvaluate(3)
      │       │
      │       ├─► Level 1 (User Turn):
      │       │   ├─► Try user's best 3 moves
      │       │   └─► Pick minimum score
      │       │
      │       ├─► Level 2 (Computer Turn):
      │       │   ├─► Try computer's best 3 moves
      │       │   └─► Pick maximum score
      │       │
      │       └─► Level 3 (User Turn):
      │           ├─► Try user's best 3 moves
      │           └─► Return minimum score
      │
      ├─► Score = computerScore - userScore
      │
      ├─► Restore State (undo simulation)
      │
      └─► Track if best score
      
      Repeat for Clusters #2-6...
      
      Return cluster with highest score
```

## Data Flow

```
User Input → GUI Layer → Game Logic → Graph Structure
  │             │            │              │
  │             │            │              └─► Node updates
  │             │            │
  │             │            └─► Score/State updates
  │             │
  │             └─► Visual feedback
  │
  └─► Hover: Show cluster preview
      Click: Execute move
      
Computer AI → Game Logic → Graph Structure
      │           │              │
      │           │              └─► Simulate moves
      │           │
      │           └─► Evaluate positions
      │
      └─► Choose best move → Execute

Graph Updates → Gravity → Neighbor Updates
      │            │             │
      │            │             └─► Rebuild adjacency lists
      │            │
      │            └─► Compact grid
      │
      └─► Mark nodes inactive
```

## Class Relationships

```
┌─────────────────────────────────────────────────────────┐
│                      main.cpp                           │
│                                                         │
│  ┌───────────────────────────────────────────────────┐ │
│  │              SameGameGUI                          │ │
│  │                                                   │ │
│  │  Private:                                         │ │
│  │    - SDL_Window* window                          │ │
│  │    - SDL_Renderer* renderer                      │ │
│  │    - TTF_Font* font                              │ │
│  │    - SameGame* game ───────────────────┐         │ │
│  │    - hoveredCluster                    │         │ │
│  │    - gameOver, gameWon                 │         │ │
│  │                                         │         │ │
│  │  Public:                                │         │ │
│  │    + init()                             │         │ │
│  │    + run()                              │         │ │
│  │    + render()                           │         │ │
│  │    + handleMouseMove()                  │         │ │
│  │    + handleMouseClick()                 │         │ │
│  │    + executeComputerMove()              │         │ │
│  └────────────────────────────────┬────────┘         │ │
│                                   │                   │ │
│                                   │ Calls methods     │ │
│                                   │                   │ │
└───────────────────────────────────┼───────────────────┘ │
                                    │                     │
                                    ▼                     │
┌─────────────────────────────────────────────────────────┘
│                  SameGame.h / SameGame.cpp              
│                                                         
│  ┌───────────────────────────────────────────────────┐ 
│  │                 SameGame                          │ 
│  │                                                   │ 
│  │  Private:                                         │ 
│  │    - vector<Node> nodes ──────────────┐          │ 
│  │    - vector<vector<int>> nodeGrid     │          │ 
│  │    - rows, cols                       │          │ 
│  │    - score, moves                     │          │ 
│  │    - isUserTurn                       │          │ 
│  │    - userScore, computerScore         │          │ 
│  │    - dpMemo                           │          │ 
│  │                                        │          │ 
│  │  Private Methods:                     │          │ 
│  │    - detectClusterBFS()               │          │ 
│  │    - applyGravity()                   │          │ 
│  │    - buildGraph()                     │          │ 
│  │    - updateNeighbors()                │          │ 
│  │    - dpEvaluate()                     │          │ 
│  │    - saveState() / restoreState()     │          │ 
│  │                                        │          │ 
│  │  Public Methods:                      │          │ 
│  │    + getTile()                        │          │ 
│  │    + isTileActive()                   │          │ 
│  │    + getCluster()                     │          │ 
│  │    + removeCluster()                  │          │ 
│  │    + hasMovesLeft()                   │          │ 
│  │    + getBestMove() ← AI              │          │ 
│  └────────────────────────────────────────┘          │ 
│                                                       │ 
│  ┌───────────────────────────────────────────────────┘ 
│  │                    Node                           
│  │                                                   
│  │    int row, col                                  
│  │    char color                                    
│  │    bool active                                   
│  │    vector<int> neighbors                         
│  └───────────────────────────────────────────────── 
```

## Memory Layout

```
Graph Structure in Memory:

nodes vector:
┌────────────────────────────────────────────────────┐
│ [0]           [1]           [2]         ...  [47]  │
│ Node(0,0,G)   Node(0,1,W)   Node(0,2,R)     Node   │
└────────────────────────────────────────────────────┘
      │
      └─► neighbors: [1, 8] (right and down)

nodeGrid (6x8):
┌───┬───┬───┬───┬───┬───┬───┬───┐
│ 0 │ 1 │ 2 │ 3 │ 4 │ 5 │ 6 │ 7 │  Row 0
├───┼───┼───┼───┼───┼───┼───┼───┤
│ 8 │ 9 │10 │11 │12 │13 │14 │15 │  Row 1
├───┼───┼───┼───┼───┼───┼───┼───┤
│16 │17 │18 │19 │20 │21 │22 │23 │  Row 2
├───┼───┼───┼───┼───┼───┼───┼───┤
│24 │25 │26 │27 │28 │29 │30 │31 │  Row 3
├───┼───┼───┼───┼───┼───┼───┼───┤
│32 │33 │34 │35 │36 │37 │38 │39 │  Row 4
├───┼───┼───┼───┼───┼───┼───┼───┤
│40 │41 │42 │43 │44 │45 │46 │47 │  Row 5
└───┴───┴───┴───┴───┴───┴───┴───┘

After Gravity (example):
- Inactive nodes stay in nodes vector
- nodeGrid[i][j] = -1 for empty positions
- Active nodes repositioned, neighbors rebuilt
```

## Event Flow Timeline

```
Time  Event                        Action
═══════════════════════════════════════════════════════════
0ms   Application Start           Initialize SDL, create game
10ms  First Render                Display initial grid
15ms  User hovers (1,2)          Highlight white cluster (6 tiles)
200ms User clicks (1,2)           Remove white cluster
201ms   └─► detectClusterBFS      Find 6 tiles
202ms   └─► Mark inactive          Set active=false for 6 nodes
203ms   └─► Calculate score        (6-2)² = 16 points
204ms   └─► userScore += 16        User: 16, Computer: 0
205ms   └─► Switch turn            isUserTurn = false
206ms   └─► applyGravity           Tiles fall, columns shift
210ms   └─► Render update          Show new board state
3210ms Computer's turn             3 second delay elapsed
3211ms   └─► getBestMove           AI evaluation starts
3215ms     └─► getAllClusters      Find 8 clusters
3216ms     └─► Sort by size        Largest first
3217ms     └─► Evaluate top 6      Minimax with DP
3350ms     └─► Return best         (2,3)
3351ms   └─► removeCluster(2,3)    Execute AI move
3352ms     └─► ... (same as user)  Score, gravity, switch
3355ms   └─► Render update         Show AI's move
3356ms User's turn again           User can click now
...
```

## Summary

This visualization shows:
1. **Startup**: How the application initializes
2. **Game Loop**: Continuous event-render cycle
3. **Move Flow**: User and computer move execution
4. **Cluster Removal**: BFS-based tile matching
5. **Gravity**: Two-phase compaction algorithm
6. **AI**: Minimax decision tree with pruning
7. **Data Flow**: Information flow through layers
8. **Relationships**: How classes interact
9. **Memory**: Graph structure layout
10. **Timeline**: Real-time event sequence

For more details, see:
- `README.md` - Full documentation
- `CODEBASE_SUMMARY.md` - Quick reference
- `GRAPH_STRUCTURE.md` - Graph implementation details
- Source code - `SameGame.h`, `SameGame.cpp`, `main.cpp`
