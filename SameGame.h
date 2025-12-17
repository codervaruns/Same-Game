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
    
    // User vs Computer mode
    bool isUserTurn;  // true = user's turn, false = computer's turn
    int userScore;
    int computerScore;

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
    bool getUserTurn() const { return isUserTurn; }
    int getUserScore() const { return userScore; }
    int getComputerScore() const { return computerScore; }
    
    // Game logic
    vector<pair<int, int>> getCluster(int row, int col);
    int getClusterSize(int row, int col);
    bool removeCluster(int row, int col);
    bool hasMovesLeft();
    void reset(const vector<vector<char>>& initialGrid);
    void switchTurn() { isUserTurn = !isUserTurn; }
    
    // Get all available clusters
    vector<tuple<int, char, int, int>> getAllClusters();
    
    // Computer AI - greedy algorithm
    pair<int, int> getBestMove();  // Returns {row, col} of best move, or {-1, -1} if no moves
};

#endif
