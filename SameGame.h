#ifndef SAMEGAME_H
#define SAMEGAME_H

#include <vector>
#include <queue>
#include <tuple>
#include <utility>
#include <unordered_map>
#include <unordered_set>

using namespace std;

// Node structure for graph representation
struct Node {
    int row;
    int col;
    char color;
    bool active;
    vector<int> neighbors;  // indices of adjacent nodes
    
    Node(int r, int c, char clr) : row(r), col(c), color(clr), active(true) {}
};

class SameGame {
private:
    vector<Node> nodes;  // Graph nodes
    unordered_map<int, unordered_map<int, int>> posToNodeIndex;  // maps (row, col) to node index
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
    void buildGraph(const vector<vector<char>>& initialGrid);
    int getNodeIndex(int row, int col) const;
    void updateNeighbors();
    
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
