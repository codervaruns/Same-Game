package com.samegame;

public class TestGraph {
    
    private static void printGrid(SameGame game) {
        System.out.println("Current Grid:");
        for (int i = 0; i < game.getRows(); i++) {
            for (int j = 0; j < game.getCols(); j++) {
                if (game.isTileActive(i, j)) {
                    System.out.print(game.getTile(i, j) + " ");
                } else {
                    System.out.print(". ");
                }
            }
            System.out.println();
        }
        System.out.println();
    }
    
    public static void main(String[] args) {
        System.out.println("Testing Graph-based SameGame implementation...\n");
        
        // Test 1: Basic initialization
        System.out.println("Test 1: Basic initialization");
        char[][] grid1 = {
            {'G', 'G', 'W', 'R'},
            {'G', 'W', 'W', 'R'},
            {'W', 'W', 'W', 'R'}
        };
        
        SameGame game = new SameGame(grid1);
        assert game.getRows() == 3;
        assert game.getCols() == 4;
        assert game.getScore() == 0;
        assert game.getMoves() == 0;
        
        printGrid(game);
        System.out.println("✓ Initialization test passed\n");
        
        // Test 2: getTile and isTileActive
        System.out.println("Test 2: getTile and isTileActive");
        assert game.getTile(0, 0) == 'G';
        assert game.getTile(0, 2) == 'W';
        assert game.getTile(2, 3) == 'R';
        assert game.isTileActive(0, 0) == true;
        assert game.isTileActive(1, 1) == true;
        System.out.println("✓ getTile and isTileActive tests passed\n");
        
        // Test 3: Cluster detection
        System.out.println("Test 3: Cluster detection");
        int clusterSize_0_0 = game.getClusterSize(0, 0);
        System.out.println("Cluster size at (0,0) [Green]: " + clusterSize_0_0);
        assert clusterSize_0_0 == 3; // 3 Green tiles connected
        
        int clusterSize_1_2 = game.getClusterSize(1, 2);
        System.out.println("Cluster size at (1,2) [White]: " + clusterSize_1_2);
        assert clusterSize_1_2 == 6; // 6 White tiles connected
        
        int clusterSize_0_3 = game.getClusterSize(0, 3);
        System.out.println("Cluster size at (0,3) [Red]: " + clusterSize_0_3);
        assert clusterSize_0_3 == 3; // 3 Red tiles connected
        System.out.println("✓ Cluster detection tests passed\n");
        
        // Test 4: Remove cluster and gravity
        System.out.println("Test 4: Remove cluster and gravity");
        int scoreBeforeRemove = game.getScore();
        boolean removed = game.removeCluster(1, 2); // Remove white cluster (size 6)
        assert removed == true;
        
        int expectedPoints = (6 - 2) * (6 - 2); // (size - 2)^2 = 16
        assert game.getScore() == scoreBeforeRemove + expectedPoints;
        assert game.getMoves() == 1;
        
        printGrid(game);
        System.out.println("Score: " + game.getScore() + ", Moves: " + game.getMoves());
        System.out.println("✓ Remove cluster test passed\n");
        
        // Test 5: Check tiles after gravity
        System.out.println("Test 5: Check tiles after gravity");
        // After removing white cluster, tiles should fall and shift left
        // Green tiles should be at bottom left
        assert game.isTileActive(2, 0) == true;
        assert game.getTile(2, 0) == 'G';
        System.out.println("✓ Gravity test passed\n");
        
        // Test 6: getAllClusters
        System.out.println("Test 6: getAllClusters");
        java.util.List<SameGame.ClusterInfo> clusters = game.getAllClusters();
        System.out.println("Found " + clusters.size() + " clusters:");
        for (SameGame.ClusterInfo cluster : clusters) {
            System.out.println("  Size: " + cluster.size + ", Color: " + cluster.color +
                             ", Position: (" + cluster.row + ", " + cluster.col + ")");
        }
        assert clusters.size() >= 2; // Should have at least Green and Red clusters
        System.out.println("✓ getAllClusters test passed\n");
        
        // Test 7: hasMovesLeft
        System.out.println("Test 7: hasMovesLeft");
        boolean hasMove = game.hasMovesLeft();
        System.out.println("Has moves left: " + (hasMove ? "Yes" : "No"));
        assert hasMove == true;
        System.out.println("✓ hasMovesLeft test passed\n");
        
        // Test 8: Reset game
        System.out.println("Test 8: Reset game");
        game.reset(grid1);
        assert game.getScore() == 0;
        assert game.getMoves() == 0;
        assert game.getTile(0, 0) == 'G';
        assert game.isTileActive(1, 2) == true;
        printGrid(game);
        System.out.println("✓ Reset test passed\n");
        
        // Test 9: Remove single tile (should fail)
        System.out.println("Test 9: Attempt to remove single tile");
        char[][] grid2 = {
            {'G', 'W', 'R'},
            {'W', 'R', 'G'},
            {'R', 'G', 'W'}
        };
        game.reset(grid2);
        printGrid(game);
        boolean removedSingle = game.removeCluster(0, 0); // Try to remove single Green
        assert removedSingle == false;
        assert game.getScore() == 0;
        assert game.getMoves() == 0;
        System.out.println("✓ Single tile removal correctly rejected\n");
        
        // Test 10: User vs Computer mode
        System.out.println("Test 10: User vs Computer mode");
        game.reset(grid1);
        assert game.getUserTurn() == true;
        assert game.getUserScore() == 0;
        assert game.getComputerScore() == 0;
        
        // User makes a move
        game.removeCluster(0, 0); // Remove green cluster
        assert game.getUserTurn() == false; // Should switch to computer
        assert game.getUserScore() > 0;
        
        // Computer makes a move
        SameGame.Pair<Integer, Integer> bestMove = game.getBestMove();
        assert bestMove.getFirst() != -1; // Should find a move
        game.removeCluster(bestMove.getFirst(), bestMove.getSecond());
        assert game.getUserTurn() == true; // Should switch back to user
        assert game.getComputerScore() > 0;
        
        System.out.println("User score: " + game.getUserScore());
        System.out.println("Computer score: " + game.getComputerScore());
        System.out.println("✓ User vs Computer mode test passed\n");
        
        System.out.println("==================================");
        System.out.println("All tests passed! ✓");
        System.out.println("Graph-based implementation working correctly!");
        System.out.println("==================================");
    }
}
