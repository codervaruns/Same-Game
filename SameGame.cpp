#include "SameGame.h"
#include <algorithm>

SameGame::SameGame(const vector<vector<char>>& initialGrid) {
    reset(initialGrid);
}

void SameGame::reset(const vector<vector<char>>& initialGrid) {
    grid = initialGrid;
    rows = grid.size();
    cols = rows > 0 ? grid[0].size() : 0;
    active = vector<vector<bool>>(rows, vector<bool>(cols, true));
    score = 0;
    moves = 0;
    isUserTurn = true;  // User goes first
    userScore = 0;
    computerScore = 0;
}

char SameGame::getTile(int row, int col) const {
    if (row >= 0 && row < rows && col >= 0 && col < cols) {
        return grid[row][col];
    }
    return '\0';
}

bool SameGame::isTileActive(int row, int col) const {
    if (row >= 0 && row < rows && col >= 0 && col < cols) {
        return active[row][col];
    }
    return false;
}

vector<pair<int, int>> SameGame::detectClusterBFS(int startRow, int startCol) {
    vector<pair<int, int>> cluster;
    
    if (!isTileActive(startRow, startCol)) {
        return cluster;
    }
    
    char color = grid[startRow][startCol];
    vector<vector<bool>> visited(rows, vector<bool>(cols, false));
    queue<pair<int, int>> q;
    
    q.push({startRow, startCol});
    visited[startRow][startCol] = true;
    cluster.push_back({startRow, startCol});
    
    vector<vector<int>> directions = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};
    
    while (!q.empty()) {
        pair<int, int> current = q.front();
        q.pop();
        int x = current.first;
        int y = current.second;
        
        for (const auto& dir : directions) {
            int newX = x + dir[0];
            int newY = y + dir[1];
            
            if (newX >= 0 && newX < rows && newY >= 0 && newY < cols &&
                !visited[newX][newY] && active[newX][newY] && 
                grid[newX][newY] == color) {
                
                visited[newX][newY] = true;
                q.push({newX, newY});
                cluster.push_back({newX, newY});
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
        active[tile.first][tile.second] = false;
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
            if (active[i][j]) {
                if (i != pos) {
                    grid[pos][j] = grid[i][j];
                    active[pos][j] = true;
                    active[i][j] = false;
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
            if (active[i][j]) {
                hasTiles = true;
                break;
            }
        }
        
        if (hasTiles) {
            if (j != col) {
                for (int i = 0; i < rows; i++) {
                    grid[i][col] = grid[i][j];
                    active[i][col] = active[i][j];
                    active[i][j] = false;
                }
            }
            col++;
        }
    }
}

bool SameGame::hasMovesLeft() {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (active[i][j]) {
                int clusterSize = getClusterSize(i, j);
                if (clusterSize >= 2) {
                    return true;
                }
            }
        }
    }
    return false;
}

vector<tuple<int, char, int, int>> SameGame::getAllClusters() {
    vector<tuple<int, char, int, int>> clusters;
    vector<vector<bool>> visited(rows, vector<bool>(cols, false));
    
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (active[i][j] && !visited[i][j]) {
                vector<pair<int, int>> cluster = detectClusterBFS(i, j);
                
                // Mark all tiles in cluster as visited
                for (const auto& tile : cluster) {
                    visited[tile.first][tile.second] = true;
                }
                
                // Only include clusters of size >= 2
                if (cluster.size() >= 2) {
                    clusters.push_back({cluster.size(), grid[i][j], i, j});
                }
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
