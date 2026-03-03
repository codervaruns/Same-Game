#ifndef SAMEGAME_H
#define SAMEGAME_H

#include <vector>
#include <queue>
#include <tuple>
#include <utility>
#include <unordered_map>
#include <unordered_set>
#include <string>

using namespace std;

struct Node {
    int row;
    int col;
    char color;
    bool active;
    vector<int> neighbors;
    
    Node(int r, int c, char clr) : row(r), col(c), color(clr), active(true) {}
};

class SameGame {
private:
    vector<Node> nodes;
    vector<vector<int>> nodeGrid;
    int rows;
    int cols;
    int score;
    int moves;
    
    bool isUserTurn;
    int userScore;
    int computerScore;

    vector<pair<int, int>> detectClusterBFS(int startRow, int startCol);
    void applyGravity();
    void buildGraph(const vector<vector<char>>& initialGrid);
    int getNodeIndex(int row, int col) const;
    void updateNeighbors();

    unordered_map<string, int> dpMemo;
    string boardStateKey();
    int dpEvaluate(int depth);
    int evaluatePosition();
    int getMoveHeuristic(int clusterSize, int row, int col, char color);
    
    struct BoardSnapshot {
        vector<Node> nodes;
        vector<vector<int>> nodeGrid;
        int score, moves;
        bool isUserTurn;
        int userScore, computerScore;
    };
    BoardSnapshot saveState();
    void restoreState(const BoardSnapshot& snap);
    
public:
    SameGame(const vector<vector<char>>& initialGrid);
    int getRows() const { return rows; }
    int getCols() const { return cols; }
    int getScore() const { return score; }
    int getMoves() const { return moves; }
    char getTile(int row, int col) const;
    bool isTileActive(int row, int col) const;
    bool getUserTurn() const { return isUserTurn; }
    int getUserScore() const { return userScore; }
    int getComputerScore() const { return computerScore; }
    
    vector<pair<int, int>> getCluster(int row, int col);
    int getClusterSize(int row, int col);
    bool removeCluster(int row, int col);
    bool hasMovesLeft();
    void reset(const vector<vector<char>>& initialGrid);
    void switchTurn() { isUserTurn = !isUserTurn; }
    
    vector<tuple<int, char, int, int>> getAllClusters();
    
    pair<int, int> getBestMove();
};

#endif
