#include "SameGame.h"
#include <algorithm>
#include <climits>

SameGame::SameGame(const vector<vector<char>>& initialGrid) {
    reset(initialGrid);
}

void SameGame::buildGraph(const vector<vector<char>>& initialGrid) {
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
    for (auto& node : nodes) {
        node.neighbors.clear();
    }
    
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
    if (row >= 0 && row < rows && col >= 0 && col < cols) {
        return nodeGrid[row][col];
    }
    return -1;
}

void SameGame::reset(const vector<vector<char>>& initialGrid) {
    buildGraph(initialGrid);
    score = 0;
    moves = 0;
    isUserTurn = true;
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
    
    for (const auto& tile : cluster) {
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
                    if (nodeIdx != -1) {
                        nodeGrid[i][j] = -1;
                        nodeGrid[i][col] = nodeIdx;
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
            
            for (const auto& tile : cluster) {
                int idx = getNodeIndex(tile.first, tile.second);
                if (idx != -1) {
                    visited.insert(idx);
                }
            }
            
            if (cluster.size() >= 2) {
                clusters.push_back({(int)cluster.size(), node.color, node.row, node.col});
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

void SameGame::restoreState(const BoardSnapshot& snap) {
    nodes = snap.nodes;
    nodeGrid = snap.nodeGrid;
    score = snap.score;
    moves = snap.moves;
    isUserTurn = snap.isUserTurn;
    userScore = snap.userScore;
    computerScore = snap.computerScore;
}



string SameGame::boardStateKey() {
    string key;
    key.reserve(rows * cols + 10);
    key += (isUserTurn ? 'U' : 'C');
    
    int scoreDiff = computerScore - userScore;
    key += to_string(scoreDiff) + "|";
    
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

int SameGame::getMoveHeuristic(int clusterSize, int row, int col, char color) {
    int score = 0;
    
    score += (clusterSize - 2) * (clusterSize - 2) * 10;
    
    score += (rows - row) * 2;
    
    int centerDist = abs(col - cols / 2);
    score += (cols - centerDist);
    
    int cascadePotential = 0;
    vector<vector<int>> directions = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};
    for (const auto& dir : directions) {
        int newRow = row + dir[0];
        int newCol = col + dir[1];
        if (newRow >= 0 && newRow < rows && newCol >= 0 && newCol < cols) {
            int idx = nodeGrid[newRow][newCol];
            if (idx != -1 && nodes[idx].active && nodes[idx].color != color) {
                cascadePotential++;
            }
        }
    }
    score += cascadePotential * 3;
    
    return score;
}

int SameGame::dpEvaluate(int depth) {
    if (!hasMovesLeft()) {
        int activeTiles = 0;
        for (const auto& node : nodes) {
            if (node.active) activeTiles++;
        }
        int clearBonus = (activeTiles == 0) ? 1000 : -activeTiles * 5;
        return computerScore - userScore + clearBonus;
    }
    
    if (depth <= 0) {
        return evaluatePosition();
    }
    
    string key = boardStateKey();
    auto it = dpMemo.find(key);
    if (it != dpMemo.end()) return it->second;
    
    vector<tuple<int, char, int, int>> clusters = getAllClusters();
    
    vector<pair<int, int>> moveScores;
    for (int i = 0; i < (int)clusters.size(); i++) {
        int h = getMoveHeuristic(get<0>(clusters[i]), get<2>(clusters[i]), 
                                  get<3>(clusters[i]), get<1>(clusters[i]));
        moveScores.push_back({h, i});
    }
    sort(moveScores.begin(), moveScores.end(), greater<pair<int, int>>());
    
    int bestVal = isUserTurn ? INT_MAX : INT_MIN;
    
    for (const auto& [heuristic, idx] : moveScores) {
        BoardSnapshot snap = saveState();
        removeCluster(get<2>(clusters[idx]), get<3>(clusters[idx]));
        
        int val = dpEvaluate(depth - 1);
        
        if (snap.isUserTurn) {
            bestVal = min(bestVal, val);
        } else {
            bestVal = max(bestVal, val);
        }
        
        restoreState(snap);
    }
    
    dpMemo[key] = bestVal;
    return bestVal;
}

int SameGame::evaluatePosition() {
    int eval = computerScore - userScore;
    
    int activeTiles = 0;
    unordered_map<char, int> colorCount;
    
    for (const auto& node : nodes) {
        if (node.active) {
            activeTiles++;
            colorCount[node.color]++;
        }
    }
    
    for (const auto& node : nodes) {
        if (node.active) {
            bool hasNeighbor = false;
            for (int neighborIdx : node.neighbors) {
                if (nodes[neighborIdx].active && 
                    nodes[neighborIdx].color == node.color) {
                    hasNeighbor = true;
                    break;
                }
            }
            if (!hasNeighbor) eval -= 3;
        }
    }
    
    vector<tuple<int, char, int, int>> clusters = getAllClusters();
    for (const auto& [size, color, r, c] : clusters) {
        eval += (size - 2) * (size - 2);
    }
    
    return eval;
}

pair<int, int> SameGame::getBestMove() {
    dpMemo.clear();
    vector<tuple<int, char, int, int>> clusters = getAllClusters();
    if (clusters.empty()) return {-1, -1};
    
    int lookDepth;
    int numClusters = clusters.size();
    if (numClusters <= 4) {
        lookDepth = 6;
    } else if (numClusters <= 8) {
        lookDepth = 4;
    } else {
        lookDepth = 3;
    }
    
    vector<pair<int, int>> moveScores;
    for (int i = 0; i < (int)clusters.size(); i++) {
        int h = getMoveHeuristic(get<0>(clusters[i]), get<2>(clusters[i]), 
                                  get<3>(clusters[i]), get<1>(clusters[i]));
        moveScores.push_back({h, i});
    }
    sort(moveScores.begin(), moveScores.end(), greater<pair<int, int>>());
    
    int bestScore = INT_MIN;
    pair<int, int> bestMove = {-1, -1};
    
    for (const auto& [heuristic, idx] : moveScores) {
        BoardSnapshot snap = saveState();
        int clusterRow = get<2>(clusters[idx]);
        int clusterCol = get<3>(clusters[idx]);
        int clusterSize = get<0>(clusters[idx]);
        
        removeCluster(clusterRow, clusterCol);
        
        int score = dpEvaluate(lookDepth);
        
        if (score > bestScore || 
            (score == bestScore && clusterSize > get<0>(clusters[moveScores[0].second]))) {
            bestScore = score;
            bestMove = {clusterRow, clusterCol};
        }
        
        restoreState(snap);
    }
    
    return bestMove;
}