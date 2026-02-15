#include "SameGame.h"
#include <algorithm>

SameGame::SameGame(const vector<vector<char>>& initialGrid) {
    reset(initialGrid);
}

void SameGame::buildGraph(const vector<vector<char>>& initialGrid) {
    nodes.clear();
    posToNodeIndex.clear();
    
    rows = initialGrid.size();
    cols = rows > 0 ? initialGrid[0].size() : 0;
    
    // Create nodes for each tile
    int index = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            nodes.push_back(Node(i, j, initialGrid[i][j]));
            posToNodeIndex[i][j] = index;
            index++;
        }
    }
    
    // Build adjacency relationships
    updateNeighbors();
}

void SameGame::updateNeighbors() {
    // Clear all neighbor lists
    for (auto& node : nodes) {
        node.neighbors.clear();
    }
    
    // Rebuild neighbors based on current positions
    vector<vector<int>> directions = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};
    
    for (auto& node : nodes) {
        if (!node.active) continue;
        
        int row = node.row;
        int col = node.col;
        
        for (const auto& dir : directions) {
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
    auto rowIt = posToNodeIndex.find(row);
    if (rowIt != posToNodeIndex.end()) {
        auto colIt = rowIt->second.find(col);
        if (colIt != rowIt->second.end()) {
            return colIt->second;
        }
    }
    return -1;
}

void SameGame::reset(const vector<vector<char>>& initialGrid) {
    buildGraph(initialGrid);
    score = 0;
    moves = 0;
    isUserTurn = true;  // User goes first
    userScore = 0;
    computerScore = 0;
    dpMemo.clear();
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
        
        Node& currentNode = nodes[currentIdx];
        int x = currentNode.row;
        int y = currentNode.col;
        
        for (const auto& dir : directions) {
            int newX = x + dir[0];
            int newY = y + dir[1];
            
            int neighborIdx = getNodeIndex(newX, newY);
            if (neighborIdx != -1 && visited.find(neighborIdx) == visited.end() &&
                nodes[neighborIdx].active && nodes[neighborIdx].color == color) {
                
                visited.insert(neighborIdx);
                q.push(neighborIdx);
                cluster.push_back({nodes[neighborIdx].row, nodes[neighborIdx].col});
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
    
    // Only remove clusters of size 2 or more
    if (cluster.size() < 2) {
        return false;
    }
    
    // Remove tiles in cluster
    for (const auto& tile : cluster) {
        int nodeIdx = getNodeIndex(tile.first, tile.second);
        if (nodeIdx != -1) {
            nodes[nodeIdx].active = false;
        }
    }
    
    // Calculate score: (size - 2)^2
    int clusterSize = cluster.size();
    int points = (clusterSize - 2) * (clusterSize - 2);
    score += points;
    
    // Add to appropriate player's score
    if (isUserTurn) {
        userScore += points;
    } else {
        computerScore += points;
    }
    
    moves++;
    
    // Switch turns after successful move
    switchTurn();
    
    // Apply gravity
    applyGravity();
    
    return true;
}

void SameGame::applyGravity() {
    // Vertical gravity: tiles fall down
    for (int j = 0; j < cols; j++) {
        int pos = rows - 1;
        for (int i = rows - 1; i >= 0; i--) {
            int nodeIdx = getNodeIndex(i, j);
            if (nodeIdx != -1 && nodes[nodeIdx].active) {
                if (i != pos) {
                    // Move node to new position
                    Node& node = nodes[nodeIdx];
                    
                    // Update position mapping
                    posToNodeIndex[i].erase(j);
                    posToNodeIndex[pos][j] = nodeIdx;
                    
                    // Update node position
                    node.row = pos;
                    node.col = j;
                }
                pos--;
            }
        }
    }
    
    // Horizontal gravity: empty columns shift left
    int col = 0;
    for (int j = 0; j < cols; j++) {
        bool hasTiles = false;
        for (int i = 0; i < rows; i++) {
            int nodeIdx = getNodeIndex(i, j);
            if (nodeIdx != -1 && nodes[nodeIdx].active) {
                hasTiles = true;
                break;
            }
        }
        
        if (hasTiles) {
            if (j != col) {
                for (int i = 0; i < rows; i++) {
                    int nodeIdx = getNodeIndex(i, j);
                    if (nodeIdx != -1) {
                        Node& node = nodes[nodeIdx];
                        
                        // Update position mapping
                        posToNodeIndex[i].erase(j);
                        posToNodeIndex[i][col] = nodeIdx;
                        
                        // Update node position
                        node.col = col;
                    }
                }
            }
            col++;
        }
    }
    
    // Update neighbor relationships after gravity
    updateNeighbors();
}

bool SameGame::hasMovesLeft() {
    for (const auto& node : nodes) {
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
        const auto& node = nodes[nodeIdx];
        if (node.active && visited.find(nodeIdx) == visited.end()) {
            vector<pair<int, int>> cluster = detectClusterBFS(node.row, node.col);
            
            // Mark all tiles in cluster as visited
            for (const auto& tile : cluster) {
                int idx = getNodeIndex(tile.first, tile.second);
                if (idx != -1) {
                    visited.insert(idx);
                }
            }
            
            // Only include clusters of size >= 2
            if (cluster.size() >= 2) {
                clusters.push_back({(int)cluster.size(), node.color, node.row, node.col});
            }
        }
    }
    
    return clusters;
}

// ============================================================
//  SNAPSHOT / RESTORE — used by DP to simulate moves
// ============================================================

SameGame::BoardSnapshot SameGame::saveState() {
    BoardSnapshot snap;
    snap.nodes = nodes;
    snap.posToNodeIndex = posToNodeIndex;
    snap.score = score;
    snap.moves = moves;
    snap.isUserTurn = isUserTurn;
    snap.userScore = userScore;
    snap.computerScore = computerScore;
    return snap;
}

void SameGame::restoreState(const BoardSnapshot& snap) {
    nodes = snap.nodes;
    posToNodeIndex = snap.posToNodeIndex;
    score = snap.score;
    moves = snap.moves;
    isUserTurn = snap.isUserTurn;
    userScore = snap.userScore;
    computerScore = snap.computerScore;
}

// ============================================================
//  DIVIDE & CONQUER — find clusters by splitting column range
// ============================================================

vector<tuple<int, char, int, int>> SameGame::findClustersDnC(int colLeft, int colRight) {
    // Base case: single column — scan each row for clusters starting here
    if (colLeft == colRight) {
        vector<tuple<int, char, int, int>> result;
        unordered_set<int> visited;
        
        for (int r = 0; r < rows; r++) {
            int idx = getNodeIndex(r, colLeft);
            if (idx != -1 && nodes[idx].active && visited.find(idx) == visited.end()) {
                vector<pair<int, int>> cluster = detectClusterBFS(r, colLeft);
                for (const auto& tile : cluster) {
                    int tIdx = getNodeIndex(tile.first, tile.second);
                    if (tIdx != -1) visited.insert(tIdx);
                }
                if (cluster.size() >= 2) {
                    result.push_back({(int)cluster.size(), nodes[idx].color, r, colLeft});
                }
            }
        }
        return result;
    }
    
    // Divide: split column range into two halves
    int mid = colLeft + (colRight - colLeft) / 2;
    
    // Conquer: recursively find clusters in each half
    vector<tuple<int, char, int, int>> leftClusters = findClustersDnC(colLeft, mid);
    vector<tuple<int, char, int, int>> rightClusters = findClustersDnC(mid + 1, colRight);
    
    // Merge: combine results and handle boundary clusters
    // Clusters that span the boundary (mid, mid+1) may have been split.
    // We do a full BFS for tiles on the boundary columns to find merged clusters.
    unordered_set<int> boundaryVisited;
    vector<tuple<int, char, int, int>> boundaryClusters;
    
    for (int r = 0; r < rows; r++) {
        // Check tiles at boundary columns mid and mid+1
        for (int bc : {mid, mid + 1}) {
            int idx = getNodeIndex(r, bc);
            if (idx != -1 && nodes[idx].active && boundaryVisited.find(idx) == boundaryVisited.end()) {
                vector<pair<int, int>> cluster = detectClusterBFS(r, bc);
                
                // Check if this cluster spans the boundary
                bool spansLeft = false, spansRight = false;
                for (const auto& tile : cluster) {
                    int tIdx = getNodeIndex(tile.first, tile.second);
                    if (tIdx != -1) boundaryVisited.insert(tIdx);
                    if (tile.second <= mid) spansLeft = true;
                    if (tile.second > mid) spansRight = true;
                }
                
                if (spansLeft && spansRight && cluster.size() >= 2) {
                    boundaryClusters.push_back({(int)cluster.size(), nodes[idx].color, r, bc});
                }
            }
        }
    }
    
    // Build final result: use a set of (row, col) representatives to deduplicate
    // Start with boundary clusters (they override any split versions from left/right)
    unordered_set<int> mergedReps;  // node indices of cluster representatives already added
    vector<tuple<int, char, int, int>> merged;
    
    for (const auto& bc : boundaryClusters) {
        int repIdx = getNodeIndex(get<2>(bc), get<3>(bc));
        if (repIdx != -1) mergedReps.insert(repIdx);
        merged.push_back(bc);
    }
    
    // Add left/right clusters that are NOT part of a boundary-spanning cluster
    auto addIfNotMerged = [&](const vector<tuple<int, char, int, int>>& src) {
        for (const auto& c : src) {
            int cRow = get<2>(c);
            int cCol = get<3>(c);
            // Check if this cluster's representative tile is part of a boundary cluster
            int cIdx = getNodeIndex(cRow, cCol);
            if (cIdx == -1) continue;
            
            // Re-detect to see the actual cluster this tile belongs to now
            vector<pair<int, int>> fullCluster = detectClusterBFS(cRow, cCol);
            bool spansBoundary = false;
            for (const auto& tile : fullCluster) {
                if ((tile.second <= mid && tile.second >= colLeft) &&
                    (fullCluster.size() >= 2)) {
                    // check if any part goes across
                }
                if (tile.second <= mid) spansBoundary = false; // reset
                if (tile.second > mid) { spansBoundary = true; break; }
            }
            // Simpler check: just see if the representative is already covered
            if (boundaryVisited.find(cIdx) != boundaryVisited.end() && 
                mergedReps.find(cIdx) == mergedReps.end()) {
                // This cluster was absorbed by a boundary cluster, skip it
                continue;
            }
            if (mergedReps.find(cIdx) != mergedReps.end()) continue;
            
            mergedReps.insert(cIdx);
            merged.push_back(c);
        }
    };
    
    addIfNotMerged(leftClusters);
    addIfNotMerged(rightClusters);
    
    return merged;
}

// ============================================================
//  DP — memoized lookahead to evaluate board quality
// ============================================================

string SameGame::boardStateKey() {
    // Build a compact string representing active tiles
    string key;
    key.reserve(rows * cols);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int idx = getNodeIndex(i, j);
            if (idx != -1 && nodes[idx].active) {
                key += nodes[idx].color;
            } else {
                key += '.';
            }
        }
    }
    return key;
}

int SameGame::dpEvaluate(int depth) {
    // Base case: no depth left or no moves available
    if (depth <= 0 || !hasMovesLeft()) {
        return 0;
    }
    
    // Memoization: check if we already computed this state
    string key = boardStateKey();
    auto it = dpMemo.find(key);
    if (it != dpMemo.end()) {
        return it->second;
    }
    
    // Find all clusters using D&C
    vector<tuple<int, char, int, int>> clusters = findClustersDnC(0, cols - 1);
    
    if (clusters.empty()) {
        dpMemo[key] = 0;
        return 0;
    }
    
    int bestScore = 0;
    
    // Try each cluster and pick the one with best total DP score
    for (const auto& cluster : clusters) {
        int cRow = get<2>(cluster);
        int cCol = get<3>(cluster);
        int cSize = get<0>(cluster);
        int immediatePoints = (cSize - 2) * (cSize - 2);
        
        // Save state, simulate the move, recursively evaluate
        BoardSnapshot snap = saveState();
        
        // Temporarily do the removal (without switching turns or updating scores)
        vector<pair<int, int>> tiles = detectClusterBFS(cRow, cCol);
        for (const auto& tile : tiles) {
            int idx = getNodeIndex(tile.first, tile.second);
            if (idx != -1) nodes[idx].active = false;
        }
        applyGravity();
        
        int futureScore = dpEvaluate(depth - 1);
        int totalScore = immediatePoints + futureScore;
        
        if (totalScore > bestScore) {
            bestScore = totalScore;
        }
        
        // Restore state
        restoreState(snap);
    }
    
    dpMemo[key] = bestScore;
    return bestScore;
}

// ============================================================
//  getBestMove — D&C + DP + Sorting (no greedy logic)
// ============================================================

pair<int, int> SameGame::getBestMove() {
    // Clear DP memo for fresh evaluation each turn
    dpMemo.clear();
    
    // Step 1: DIVIDE & CONQUER — find all clusters
    vector<tuple<int, char, int, int>> clusters = findClustersDnC(0, cols - 1);
    
    if (clusters.empty()) {
        return {-1, -1};  // No moves available
    }
    
    // Step 2: DP — evaluate each candidate move with lookahead
    // Store (dpScore, row, col) for each candidate
    vector<tuple<int, int, int>> scoredMoves;
    
    int lookDepth = 2;  // Lookahead depth for DP
    
    for (const auto& cluster : clusters) {
        int cRow = get<2>(cluster);
        int cCol = get<3>(cluster);
        int cSize = get<0>(cluster);
        int immediatePoints = (cSize - 2) * (cSize - 2);
        
        // Simulate removal and evaluate future with DP
        BoardSnapshot snap = saveState();
        
        vector<pair<int, int>> tiles = detectClusterBFS(cRow, cCol);
        for (const auto& tile : tiles) {
            int idx = getNodeIndex(tile.first, tile.second);
            if (idx != -1) nodes[idx].active = false;
        }
        applyGravity();
        
        int futureScore = dpEvaluate(lookDepth);
        int totalScore = immediatePoints + futureScore;
        
        restoreState(snap);
        
        scoredMoves.push_back({totalScore, cRow, cCol});
    }
    
    // Step 3: SORTING — sort candidates by DP score descending
    sort(scoredMoves.begin(), scoredMoves.end(),
         [](const tuple<int, int, int>& a, const tuple<int, int, int>& b) {
             return get<0>(a) > get<0>(b);
         });
    
    // Return the highest-scoring move
    int bestRow = get<1>(scoredMoves[0]);
    int bestCol = get<2>(scoredMoves[0]);
    return {bestRow, bestCol};
}