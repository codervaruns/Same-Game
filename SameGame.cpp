#include "SameGame.h"
#include <algorithm>
#include <climits>
#include <cmath>

SameGame::SameGame(const vector<vector<char>> &initialGrid) {
  reset(initialGrid);
}

void SameGame::buildGraph(const vector<vector<char>> &initialGrid) {
  nodes.clear();

  rows = initialGrid.size();
  cols = rows > 0 ? initialGrid[0].size() : 0;

  nodeGrid.assign(rows, vector<int>(cols, -1));

  int index = 0;
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      nodes.push_back(Node(i, j, initialGrid[i][j]));
      nodeGrid[i][j] = index;
      index++;
    }
  }

  updateNeighbors();
}

void SameGame::updateNeighbors() {
  for (auto &node : nodes) {
    node.neighbors.clear();
  }

  vector<vector<int>> directions = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};

  for (auto &node : nodes) {
    if (!node.active)
      continue;

    int row = node.row;
    int col = node.col;

    for (const auto &dir : directions) {
      int newRow = row + dir[0];
      int newCol = col + dir[1];

      if (newRow >= 0 && newRow < rows && newCol >= 0 && newCol < cols) {
        int neighborIdx = getNodeIndex(newRow, newCol);
        if (neighborIdx != -1 && nodes[neighborIdx].active) {
          node.neighbors.push_back(neighborIdx);
        }
      }
    }
  }
}

int SameGame::getNodeIndex(int row, int col) const {
  if (row >= 0 && row < rows && col >= 0 && col < cols) {
    return nodeGrid[row][col];
  }
  return -1;
}

void SameGame::reset(const vector<vector<char>> &initialGrid) {
  buildGraph(initialGrid);
  score = 0;
  moves = 0;
  isUserTurn = true;
  userScore = 0;
  computerScore = 0;
  transTable.clear();
}

char SameGame::getTile(int row, int col) const {
  int nodeIdx = getNodeIndex(row, col);
  if (nodeIdx != -1) {
    return nodes[nodeIdx].color;
  }
  return '\0';
}

bool SameGame::isTileActive(int row, int col) const {
  int nodeIdx = getNodeIndex(row, col);
  if (nodeIdx != -1) {
    return nodes[nodeIdx].active;
  }
  return false;
}

vector<pair<int, int>> SameGame::detectClusterBFS(int startRow, int startCol) {
  vector<pair<int, int>> cluster;

  int startNodeIdx = getNodeIndex(startRow, startCol);
  if (startNodeIdx == -1 || !nodes[startNodeIdx].active) {
    return cluster;
  }

  char color = nodes[startNodeIdx].color;
  unordered_set<int> visited;
  queue<int> q;

  q.push(startNodeIdx);
  visited.insert(startNodeIdx);
  cluster.push_back({nodes[startNodeIdx].row, nodes[startNodeIdx].col});

  vector<vector<int>> directions = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};

  while (!q.empty()) {
    int currentIdx = q.front();
    q.pop();

    Node &currentNode = nodes[currentIdx];
    int x = currentNode.row;
    int y = currentNode.col;

    for (const auto &dir : directions) {
      int newX = x + dir[0];
      int newY = y + dir[1];

      if (newX >= 0 && newX < rows && newY >= 0 && newY < cols) {
        int neighborIdx = nodeGrid[newX][newY];
        if (neighborIdx != -1 && visited.find(neighborIdx) == visited.end() &&
            nodes[neighborIdx].active && nodes[neighborIdx].color == color) {

          visited.insert(neighborIdx);
          q.push(neighborIdx);
          cluster.push_back({nodes[neighborIdx].row, nodes[neighborIdx].col});
        }
      }
    }
  }

  return cluster;
}

vector<pair<int, int>> SameGame::getCluster(int row, int col) {
  return detectClusterBFS(row, col);
}

int SameGame::getClusterSize(int row, int col) {
  return detectClusterBFS(row, col).size();
}

bool SameGame::removeCluster(int row, int col) {
  vector<pair<int, int>> cluster = detectClusterBFS(row, col);

  if (cluster.size() < 2) {
    return false;
  }

  for (const auto &tile : cluster) {
    int nodeIdx = getNodeIndex(tile.first, tile.second);
    if (nodeIdx != -1) {
      nodes[nodeIdx].active = false;
    }
  }

  int clusterSize = cluster.size();
  int points = (clusterSize - 2) * (clusterSize - 2);
  score += points;

  if (isUserTurn) {
    userScore += points;
  } else {
    computerScore += points;
  }

  moves++;

  switchTurn();

  applyGravity();

  return true;
}

void SameGame::applyGravity() {
  for (int j = 0; j < cols; j++) {
    int pos = rows - 1;
    for (int i = rows - 1; i >= 0; i--) {
      int nodeIdx = nodeGrid[i][j];
      if (nodeIdx != -1 && nodes[nodeIdx].active) {
        if (i != pos) {
          nodeGrid[i][j] = -1;
          nodeGrid[pos][j] = nodeIdx;

          nodes[nodeIdx].row = pos;
          nodes[nodeIdx].col = j;
        }
        pos--;
      }
    }
    // Clear remaining rows above
    for (int i = pos; i >= 0; i--) {
      nodeGrid[i][j] = -1;
    }
  }

  int col = 0;
  for (int j = 0; j < cols; j++) {
    bool hasTiles = false;
    for (int i = 0; i < rows; i++) {
      int nodeIdx = nodeGrid[i][j];
      if (nodeIdx != -1 && nodes[nodeIdx].active) {
        hasTiles = true;
        break;
      }
    }

    if (hasTiles) {
      if (j != col) {
        for (int i = 0; i < rows; i++) {
          int nodeIdx = nodeGrid[i][j];
          nodeGrid[i][col] = nodeIdx;
          nodeGrid[i][j] = -1;
          if (nodeIdx != -1) {
            nodes[nodeIdx].col = col;
          }
        }
      }
      col++;
    }
  }

  updateNeighbors();
}

bool SameGame::hasMovesLeft() {
  for (const auto &node : nodes) {
    if (node.active) {
      int clusterSize = getClusterSize(node.row, node.col);
      if (clusterSize >= 2) {
        return true;
      }
    }
  }
  return false;
}

vector<tuple<int, char, int, int>> SameGame::getAllClusters() {
  vector<tuple<int, char, int, int>> clusters;
  unordered_set<int> visited;

  for (int nodeIdx = 0; nodeIdx < (int)nodes.size(); nodeIdx++) {
    const auto &node = nodes[nodeIdx];
    if (node.active && visited.find(nodeIdx) == visited.end()) {
      vector<pair<int, int>> cluster = detectClusterBFS(node.row, node.col);

      for (const auto &tile : cluster) {
        int idx = getNodeIndex(tile.first, tile.second);
        if (idx != -1) {
          visited.insert(idx);
        }
      }

      if (cluster.size() >= 2) {
        clusters.push_back(
            {(int)cluster.size(), node.color, node.row, node.col});
      }
    }
  }

  return clusters;
}

SameGame::BoardSnapshot SameGame::saveState() {
  BoardSnapshot snap;
  snap.nodes = nodes;
  snap.nodeGrid = nodeGrid;
  snap.score = score;
  snap.moves = moves;
  snap.isUserTurn = isUserTurn;
  snap.userScore = userScore;
  snap.computerScore = computerScore;
  return snap;
}

void SameGame::restoreState(const BoardSnapshot &snap) {
  nodes = snap.nodes;
  nodeGrid = snap.nodeGrid;
  score = snap.score;
  moves = snap.moves;
  isUserTurn = snap.isUserTurn;
  userScore = snap.userScore;
  computerScore = snap.computerScore;
}

// ============================================================
//                    AI ENGINE — OVERHAULED
// ============================================================

string SameGame::boardStateKey() {
  string key;
  key.reserve(rows * cols + 5);
  key += (isUserTurn ? 'U' : 'C');
  key += ':';

  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      int idx = nodeGrid[i][j];
      if (idx != -1 && nodes[idx].active) {
        key += nodes[idx].color;
      } else {
        key += '.';
      }
    }
  }
  return key;
}

// Count how many pairs of adjacent same-color active tiles exist.
// Higher = more clusters possible = more "alive" the board is.
int SameGame::countColorAdjacencies() {
  int adj = 0;
  for (const auto &node : nodes) {
    if (!node.active)
      continue;
    for (int nIdx : node.neighbors) {
      if (nodes[nIdx].active && nodes[nIdx].color == node.color) {
        adj++;
      }
    }
  }
  return adj / 2; // each edge counted twice
}

// Strategic evaluation: called when search depth is exhausted.
// Positive = good for computer, negative = good for user.
int SameGame::evaluateStrategic() {
  // 1) Raw score differential — most important signal
  int eval = (computerScore - userScore) * 100;

  // 2) Gather all available clusters
  vector<tuple<int, char, int, int>> clusters = getAllClusters();

  // Sort clusters by size descending
  sort(clusters.begin(), clusters.end(),
       [](const auto &a, const auto &b) { return get<0>(a) > get<0>(b); });

  // 3) Turn-aware cluster potential attribution:
  //    The current player gets the largest cluster, opponent gets the next,
  //    etc. This simulates the alternating-turn reality.
  bool compTurn = !isUserTurn; // from computer's perspective
  for (int i = 0; i < (int)clusters.size(); i++) {
    int sz = get<0>(clusters[i]);
    int points = (sz - 2) * (sz - 2);

    bool thisTurnIsComputer = (i % 2 == 0) ? compTurn : !compTurn;
    if (thisTurnIsComputer) {
      eval += points * 3; // weight future computer points
    } else {
      eval -= points * 3; // weight future user points
    }
  }

  // 4) Cluster-denial: penalise states where the opponent's NEXT move
  //    has a huge cluster (>= 4 tiles).
  if (!clusters.empty() && !compTurn) {
    // It's the user's turn next — if the biggest cluster is huge, bad for us
    int biggestForUser = get<0>(clusters[0]);
    if (biggestForUser >= 4) {
      eval -= biggestForUser * biggestForUser * 2;
    }
  }

  // 5) Isolated tile penalty — tiles with no same-color neighbor are dead
  // weight
  int isolatedCount = 0;
  int activeTiles = 0;
  for (const auto &node : nodes) {
    if (!node.active)
      continue;
    activeTiles++;
    bool hasNeighbor = false;
    for (int neighborIdx : node.neighbors) {
      if (nodes[neighborIdx].active && nodes[neighborIdx].color == node.color) {
        hasNeighbor = true;
        break;
      }
    }
    if (!hasNeighbor)
      isolatedCount++;
  }
  eval -= isolatedCount * 8;

  // 6) Connectivity bonus — more same-color adjacencies = healthier board
  int adjacencies = countColorAdjacencies();
  if (compTurn) {
    eval += adjacencies * 2;
  } else {
    eval -= adjacencies * 2;
  }

  // 7) Fewer active tiles remaining is better (game closer to ending)
  //    Only beneficial when computer is already winning
  if (computerScore > userScore) {
    eval += (rows * cols - activeTiles) * 1;
  }

  // 8) Board-clear bonus prediction
  if (activeTiles == 0) {
    eval += 1000;
  }

  return eval;
}

// Smart move ordering: prioritise moves that are likely best.
// Better ordering → more alpha-beta cutoffs → deeper effective search.
int SameGame::getSmartMoveHeuristic(int clusterSize, int row, int col,
                                    char color) {
  int h = 0;

  // a) Immediate points — biggest factor
  int points = (clusterSize - 2) * (clusterSize - 2);
  h += points * 1000;

  // b) Larger clusters are always more attractive even below the quadratic
  // bonus
  h += clusterSize * 200;

  // c) Prefer clusters near the bottom (gravity brings more tiles together
  // after removal)
  h += row * 30;

  // d) Prefer clusters near the center columns (more connectivity potential)
  int centerDist = abs(col - cols / 2);
  h += (cols - centerDist) * 10;

  // e) Color frequency bonus: removing a color that has many tiles
  //    increases consolidation chances for remaining colors
  int colorCount = 0;
  for (const auto &node : nodes) {
    if (node.active && node.color == color)
      colorCount++;
  }
  // Prefer removing dominant colors in large chunks
  if (clusterSize * 3 >= colorCount && colorCount >= 4) {
    h += 500; // this removes a big fraction of this color
  }

  return h;
}

// Alpha-beta minimax with transposition table.
// Positive values favor the computer, negative favor the user.
int SameGame::alphaBeta(int depth, int alpha, int beta) {
  // Terminal check
  if (!hasMovesLeft()) {
    int activeTiles = 0;
    for (const auto &node : nodes) {
      if (node.active)
        activeTiles++;
    }
    int clearBonus = (activeTiles == 0) ? 1000 : -activeTiles * 10;
    return (computerScore - userScore) * 100 + clearBonus;
  }

  // Depth exhausted — use strategic evaluation
  if (depth <= 0) {
    return evaluateStrategic();
  }

  // Transposition table lookup
  string key = boardStateKey();
  auto it = transTable.find(key);
  if (it != transTable.end()) {
    TTEntry &entry = it->second;
    if (entry.depth >= depth) {
      if (entry.flag == 0)
        return entry.value; // exact
      if (entry.flag == 1)
        alpha = max(alpha, entry.value); // lower bound
      if (entry.flag == 2)
        beta = min(beta, entry.value); // upper bound
      if (alpha >= beta)
        return entry.value;
    }
  }

  vector<tuple<int, char, int, int>> clusters = getAllClusters();

  // Move ordering: sort by heuristic score descending
  vector<pair<int, int>> moveOrder;
  for (int i = 0; i < (int)clusters.size(); i++) {
    int h = getSmartMoveHeuristic(get<0>(clusters[i]), get<2>(clusters[i]),
                                  get<3>(clusters[i]), get<1>(clusters[i]));
    moveOrder.push_back({h, i});
  }
  sort(moveOrder.begin(), moveOrder.end(), greater<pair<int, int>>());

  bool maximizing = !isUserTurn; // Computer maximizes
  int bestVal = maximizing ? INT_MIN : INT_MAX;
  int origAlpha = alpha;

  for (const auto &[heuristic, idx] : moveOrder) {
    BoardSnapshot snap = saveState();
    removeCluster(get<2>(clusters[idx]), get<3>(clusters[idx]));

    int val = alphaBeta(depth - 1, alpha, beta);

    restoreState(snap);

    if (maximizing) {
      bestVal = max(bestVal, val);
      alpha = max(alpha, val);
    } else {
      bestVal = min(bestVal, val);
      beta = min(beta, val);
    }

    // Alpha-beta cutoff
    if (alpha >= beta) {
      break;
    }
  }

  // Store in transposition table
  TTEntry entry;
  entry.value = bestVal;
  entry.depth = depth;
  if (bestVal <= origAlpha) {
    entry.flag = 2; // upper bound
  } else if (bestVal >= beta) {
    entry.flag = 1; // lower bound
  } else {
    entry.flag = 0; // exact
  }
  transTable[key] = entry;

  return bestVal;
}

pair<int, int> SameGame::getBestMove() {
  transTable.clear();
  vector<tuple<int, char, int, int>> clusters = getAllClusters();
  if (clusters.empty())
    return {-1, -1};

  // --- Quick win check: if only 1 cluster, take it ---
  if (clusters.size() == 1) {
    return {get<2>(clusters[0]), get<3>(clusters[0])};
  }

  // --- Check for immediately winning moves ---
  // A move is immediately winning if after taking it, either no moves remain
  // and computer leads, or the lead is insurmountable.
  for (int i = 0; i < (int)clusters.size(); i++) {
    BoardSnapshot snap = saveState();
    int cr = get<2>(clusters[i]);
    int cc = get<3>(clusters[i]);
    removeCluster(cr, cc);

    if (!hasMovesLeft() && computerScore > userScore) {
      restoreState(snap);
      return {cr, cc};
    }
    restoreState(snap);
  }

  // --- Adaptive search depth based on branching factor ---
  int numClusters = clusters.size();
  int lookDepth;
  if (numClusters <= 2) {
    lookDepth = 20; // exhaustive
  } else if (numClusters <= 3) {
    lookDepth = 14;
  } else if (numClusters <= 5) {
    lookDepth = 10;
  } else if (numClusters <= 8) {
    lookDepth = 8;
  } else if (numClusters <= 12) {
    lookDepth = 6;
  } else if (numClusters <= 20) {
    lookDepth = 5;
  } else {
    lookDepth = 4;
  }

  // --- Move ordering for the root ---
  vector<pair<int, int>> moveOrder;
  for (int i = 0; i < (int)clusters.size(); i++) {
    int h = getSmartMoveHeuristic(get<0>(clusters[i]), get<2>(clusters[i]),
                                  get<3>(clusters[i]), get<1>(clusters[i]));
    moveOrder.push_back({h, i});
  }
  sort(moveOrder.begin(), moveOrder.end(), greater<pair<int, int>>());

  int bestScore = INT_MIN;
  int bestClusterSize = 0;
  pair<int, int> bestMove = {-1, -1};
  int alpha = INT_MIN;
  int beta = INT_MAX;

  for (const auto &[heuristic, idx] : moveOrder) {
    BoardSnapshot snap = saveState();
    int clusterRow = get<2>(clusters[idx]);
    int clusterCol = get<3>(clusters[idx]);
    int clusterSize = get<0>(clusters[idx]);

    removeCluster(clusterRow, clusterCol);

    int val = alphaBeta(lookDepth, alpha, beta);

    restoreState(snap);

    if (val > bestScore ||
        (val == bestScore && clusterSize > bestClusterSize)) {
      bestScore = val;
      bestClusterSize = clusterSize;
      bestMove = {clusterRow, clusterCol};
    }

    alpha = max(alpha, val);
  }

  return bestMove;
}