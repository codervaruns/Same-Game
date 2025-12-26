#include "SameGame.h"
#include <iostream>
#include <cassert>

using namespace std;

void printGrid(SameGame& game) {
    cout << "Current Grid:" << endl;
    for (int i = 0; i < game.getRows(); i++) {
        for (int j = 0; j < game.getCols(); j++) {
            if (game.isTileActive(i, j)) {
                cout << game.getTile(i, j) << " ";
            } else {
                cout << ". ";
            }
        }
        cout << endl;
    }
    cout << endl;
}

int main() {
    cout << "Testing Graph-based SameGame implementation..." << endl << endl;
    
    // Test 1: Basic initialization
    cout << "Test 1: Basic initialization" << endl;
    vector<vector<char>> grid1 = {
        {'G', 'G', 'W', 'R'},
        {'G', 'W', 'W', 'R'},
        {'W', 'W', 'W', 'R'}
    };
    
    SameGame game(grid1);
    assert(game.getRows() == 3);
    assert(game.getCols() == 4);
    assert(game.getScore() == 0);
    assert(game.getMoves() == 0);
    
    printGrid(game);
    cout << "✓ Initialization test passed" << endl << endl;
    
    // Test 2: getTile and isTileActive
    cout << "Test 2: getTile and isTileActive" << endl;
    assert(game.getTile(0, 0) == 'G');
    assert(game.getTile(0, 2) == 'W');
    assert(game.getTile(2, 3) == 'R');
    assert(game.isTileActive(0, 0) == true);
    assert(game.isTileActive(1, 1) == true);
    cout << "✓ getTile and isTileActive tests passed" << endl << endl;
    
    // Test 3: Cluster detection
    cout << "Test 3: Cluster detection" << endl;
    int clusterSize_0_0 = game.getClusterSize(0, 0);
    cout << "Cluster size at (0,0) [Green]: " << clusterSize_0_0 << endl;
    assert(clusterSize_0_0 == 3); // 3 Green tiles connected
    
    int clusterSize_1_2 = game.getClusterSize(1, 2);
    cout << "Cluster size at (1,2) [White]: " << clusterSize_1_2 << endl;
    assert(clusterSize_1_2 == 6); // 6 White tiles connected
    
    int clusterSize_0_3 = game.getClusterSize(0, 3);
    cout << "Cluster size at (0,3) [Red]: " << clusterSize_0_3 << endl;
    assert(clusterSize_0_3 == 3); // 3 Red tiles connected
    cout << "✓ Cluster detection tests passed" << endl << endl;
    
    // Test 4: Remove cluster and gravity
    cout << "Test 4: Remove cluster and gravity" << endl;
    int scoreBeforeRemove = game.getScore();
    bool removed = game.removeCluster(1, 2); // Remove white cluster (size 6)
    assert(removed == true);
    
    int expectedPoints = (6 - 2) * (6 - 2); // (size - 2)^2 = 16
    assert(game.getScore() == scoreBeforeRemove + expectedPoints);
    assert(game.getMoves() == 1);
    
    printGrid(game);
    cout << "Score: " << game.getScore() << ", Moves: " << game.getMoves() << endl;
    cout << "✓ Remove cluster test passed" << endl << endl;
    
    // Test 5: Check tiles after gravity
    cout << "Test 5: Check tiles after gravity" << endl;
    // After removing white cluster, tiles should fall and shift left
    // Green tiles should be at bottom left
    assert(game.isTileActive(2, 0) == true);
    assert(game.getTile(2, 0) == 'G');
    cout << "✓ Gravity test passed" << endl << endl;
    
    // Test 6: getAllClusters
    cout << "Test 6: getAllClusters" << endl;
    vector<tuple<int, char, int, int>> clusters = game.getAllClusters();
    cout << "Found " << clusters.size() << " clusters:" << endl;
    for (const auto& cluster : clusters) {
        cout << "  Size: " << get<0>(cluster) << ", Color: " << get<1>(cluster)
             << ", Position: (" << get<2>(cluster) << ", " << get<3>(cluster) << ")" << endl;
    }
    assert(clusters.size() >= 2); // Should have at least Green and Red clusters
    cout << "✓ getAllClusters test passed" << endl << endl;
    
    // Test 7: hasMovesLeft
    cout << "Test 7: hasMovesLeft" << endl;
    bool hasMove = game.hasMovesLeft();
    cout << "Has moves left: " << (hasMove ? "Yes" : "No") << endl;
    assert(hasMove == true);
    cout << "✓ hasMovesLeft test passed" << endl << endl;
    
    // Test 8: Reset game
    cout << "Test 8: Reset game" << endl;
    game.reset(grid1);
    assert(game.getScore() == 0);
    assert(game.getMoves() == 0);
    assert(game.getTile(0, 0) == 'G');
    assert(game.isTileActive(1, 2) == true);
    printGrid(game);
    cout << "✓ Reset test passed" << endl << endl;
    
    // Test 9: Remove single tile (should fail)
    cout << "Test 9: Attempt to remove single tile" << endl;
    vector<vector<char>> grid2 = {
        {'G', 'W', 'R'},
        {'W', 'R', 'G'},
        {'R', 'G', 'W'}
    };
    game.reset(grid2);
    printGrid(game);
    bool removedSingle = game.removeCluster(0, 0); // Try to remove single Green
    assert(removedSingle == false);
    assert(game.getScore() == 0);
    assert(game.getMoves() == 0);
    cout << "✓ Single tile removal correctly rejected" << endl << endl;
    
    // Test 10: User vs Computer mode
    cout << "Test 10: User vs Computer mode" << endl;
    game.reset(grid1);
    assert(game.getUserTurn() == true);
    assert(game.getUserScore() == 0);
    assert(game.getComputerScore() == 0);
    
    // User makes a move
    game.removeCluster(0, 0); // Remove green cluster
    assert(game.getUserTurn() == false); // Should switch to computer
    assert(game.getUserScore() > 0);
    
    // Computer makes a move
    pair<int, int> bestMove = game.getBestMove();
    assert(bestMove.first != -1); // Should find a move
    game.removeCluster(bestMove.first, bestMove.second);
    assert(game.getUserTurn() == true); // Should switch back to user
    assert(game.getComputerScore() > 0);
    
    cout << "User score: " << game.getUserScore() << endl;
    cout << "Computer score: " << game.getComputerScore() << endl;
    cout << "✓ User vs Computer mode test passed" << endl << endl;
    
    cout << "==================================" << endl;
    cout << "All tests passed! ✓" << endl;
    cout << "Graph-based implementation working correctly!" << endl;
    cout << "==================================" << endl;
    
    return 0;
}
