#ifndef SAMEGAME_H
#define SAMEGAME_H

#include <vector>
#include <queue>
#include <tuple>
#include <utility>

using namespace std;

class SameGame {
private:
    vector<vector<char>> grid;
    vector<vector<bool>> active;  // true = tile exists, false = removed
    int rows;
    int cols;
    int score;
    int moves;

    // Helper functions
    vector<pair<int, int>> detectClusterBFS(int startRow, int startCol);
    void applyGravity();
    
public:
    // Constructor
    SameGame(const vector<vector<char>>& initialGrid);
    
    // Game state queries
    int getRows() const { return rows; }
    int getCols() const { return cols; }
    int getScore() const { return score; }
    int getMoves() const { return moves; }
    char getTile(int row, int col) const;
    bool isTileActive(int row, int col) const;
    
    // Game logic
    vector<pair<int, int>> getCluster(int row, int col);
    int getClusterSize(int row, int col);
    bool removeCluster(int row, int col);
    bool hasMovesLeft();
    void reset(const vector<vector<char>>& initialGrid);
    
    // Get all available clusters
    vector<tuple<int, char, int, int>> getAllClusters();
};

#endif
