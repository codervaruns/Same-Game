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
    
    for (int nodeIdx = 0; nodeIdx < nodes.size(); nodeIdx++) {
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
                clusters.push_back({cluster.size(), node.color, node.row, node.col});
            }
        }
    }
    
    return clusters;
}

pair<int, int> SameGame::getBestMove() {
    // Get all available clusters
    vector<tuple<int, char, int, int>> clusters = getAllClusters();
    
    if (clusters.empty()) {
        return {-1, -1};  // No moves available
    }
    
    // Sort by size (descending) - greedy strategy picks largest cluster
    sort(clusters.begin(), clusters.end(), 
         [](const tuple<int, char, int, int>& a, const tuple<int, char, int, int>& b) {
             return get<0>(a) > get<0>(b);
         });
    
    // Return the position of the largest cluster
    int row = get<2>(clusters[0]);
    int col = get<3>(clusters[0]);
    return {row, col};
}