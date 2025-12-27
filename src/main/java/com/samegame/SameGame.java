package com.samegame;

import java.util.*;

public class SameGame {
    private List<Node> nodes;  // Graph nodes
    private Map<Integer, Map<Integer, Integer>> posToNodeIndex;  // maps (row, col) to node index
    private int rows;
    private int cols;
    private int score;
    private int moves;
    
    // User vs Computer mode
    private boolean isUserTurn;  // true = user's turn, false = computer's turn
    private int userScore;
    private int computerScore;

    // Constructor
    public SameGame(char[][] initialGrid) {
        this.nodes = new ArrayList<>();
        this.posToNodeIndex = new HashMap<>();
        reset(initialGrid);
    }
    
    // Game state queries
    public int getRows() { return rows; }
    public int getCols() { return cols; }
    public int getScore() { return score; }
    public int getMoves() { return moves; }
    
    public char getTile(int row, int col) {
        int nodeIdx = getNodeIndex(row, col);
        if (nodeIdx != -1) {
            return nodes.get(nodeIdx).color;
        }
        return '\0';
    }
    
    public boolean isTileActive(int row, int col) {
        int nodeIdx = getNodeIndex(row, col);
        if (nodeIdx != -1) {
            return nodes.get(nodeIdx).active;
        }
        return false;
    }
    
    public boolean getUserTurn() { return isUserTurn; }
    public int getUserScore() { return userScore; }
    public int getComputerScore() { return computerScore; }
    
    // Game logic
    public List<Pair<Integer, Integer>> getCluster(int row, int col) {
        return detectClusterBFS(row, col);
    }
    
    public int getClusterSize(int row, int col) {
        return detectClusterBFS(row, col).size();
    }
    
    public boolean removeCluster(int row, int col) {
        List<Pair<Integer, Integer>> cluster = detectClusterBFS(row, col);
        
        // Only remove clusters of size 2 or more
        if (cluster.size() < 2) {
            return false;
        }
        
        // Remove tiles in cluster
        for (Pair<Integer, Integer> tile : cluster) {
            int nodeIdx = getNodeIndex(tile.getFirst(), tile.getSecond());
            if (nodeIdx != -1) {
                nodes.get(nodeIdx).active = false;
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
    
    public boolean hasMovesLeft() {
        for (Node node : nodes) {
            if (node.active) {
                int clusterSize = getClusterSize(node.row, node.col);
                if (clusterSize >= 2) {
                    return true;
                }
            }
        }
        return false;
    }
    
    public void reset(char[][] initialGrid) {
        buildGraph(initialGrid);
        score = 0;
        moves = 0;
        isUserTurn = true;  // User goes first
        userScore = 0;
        computerScore = 0;
    }
    
    public void switchTurn() { isUserTurn = !isUserTurn; }
    
    // Get all available clusters
    public List<ClusterInfo> getAllClusters() {
        List<ClusterInfo> clusters = new ArrayList<>();
        Set<Integer> visited = new HashSet<>();
        
        for (int nodeIdx = 0; nodeIdx < nodes.size(); nodeIdx++) {
            Node node = nodes.get(nodeIdx);
            if (node.active && !visited.contains(nodeIdx)) {
                List<Pair<Integer, Integer>> cluster = detectClusterBFS(node.row, node.col);
                
                // Mark all tiles in cluster as visited
                for (Pair<Integer, Integer> tile : cluster) {
                    int idx = getNodeIndex(tile.getFirst(), tile.getSecond());
                    if (idx != -1) {
                        visited.add(idx);
                    }
                }
                
                // Only include clusters of size >= 2
                if (cluster.size() >= 2) {
                    clusters.add(new ClusterInfo(cluster.size(), node.color, node.row, node.col));
                }
            }
        }
        
        return clusters;
    }
    
    // Computer AI - greedy algorithm
    public Pair<Integer, Integer> getBestMove() {
        // Get all available clusters
        List<ClusterInfo> clusters = getAllClusters();
        
        if (clusters.isEmpty()) {
            return new Pair<>(-1, -1);  // No moves available
        }
        
        // Sort by size (descending) - greedy strategy picks largest cluster
        clusters.sort((a, b) -> Integer.compare(b.size, a.size));
        
        // Return the position of the largest cluster
        ClusterInfo best = clusters.get(0);
        return new Pair<>(best.row, best.col);
    }
    
    // Helper functions
    private void buildGraph(char[][] initialGrid) {
        nodes.clear();
        posToNodeIndex.clear();
        
        rows = initialGrid.length;
        cols = rows > 0 ? initialGrid[0].length : 0;
        
        // Create nodes for each tile
        int index = 0;
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                nodes.add(new Node(i, j, initialGrid[i][j]));
                if (!posToNodeIndex.containsKey(i)) {
                    posToNodeIndex.put(i, new HashMap<>());
                }
                posToNodeIndex.get(i).put(j, index);
                index++;
            }
        }
        
        // Build adjacency relationships
        updateNeighbors();
    }
    
    private void updateNeighbors() {
        // Clear all neighbor lists
        for (Node node : nodes) {
            node.neighbors.clear();
        }
        
        // Rebuild neighbors based on current positions
        int[][] directions = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};
        
        for (Node node : nodes) {
            if (!node.active) continue;
            
            int row = node.row;
            int col = node.col;
            
            for (int[] dir : directions) {
                int newRow = row + dir[0];
                int newCol = col + dir[1];
                
                if (newRow >= 0 && newRow < rows && newCol >= 0 && newCol < cols) {
                    int neighborIdx = getNodeIndex(newRow, newCol);
                    if (neighborIdx != -1 && nodes.get(neighborIdx).active) {
                        node.neighbors.add(neighborIdx);
                    }
                }
            }
        }
    }
    
    private int getNodeIndex(int row, int col) {
        Map<Integer, Integer> rowMap = posToNodeIndex.get(row);
        if (rowMap != null) {
            Integer colIdx = rowMap.get(col);
            if (colIdx != null) {
                return colIdx;
            }
        }
        return -1;
    }
    
    private List<Pair<Integer, Integer>> detectClusterBFS(int startRow, int startCol) {
        List<Pair<Integer, Integer>> cluster = new ArrayList<>();
        
        int startNodeIdx = getNodeIndex(startRow, startCol);
        if (startNodeIdx == -1 || !nodes.get(startNodeIdx).active) {
            return cluster;
        }
        
        char color = nodes.get(startNodeIdx).color;
        Set<Integer> visited = new HashSet<>();
        Queue<Integer> q = new LinkedList<>();
        
        q.add(startNodeIdx);
        visited.add(startNodeIdx);
        cluster.add(new Pair<>(nodes.get(startNodeIdx).row, nodes.get(startNodeIdx).col));
        
        int[][] directions = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};
        
        while (!q.isEmpty()) {
            int currentIdx = q.poll();
            Node currentNode = nodes.get(currentIdx);
            int x = currentNode.row;
            int y = currentNode.col;
            
            for (int[] dir : directions) {
                int newX = x + dir[0];
                int newY = y + dir[1];
                
                int neighborIdx = getNodeIndex(newX, newY);
                if (neighborIdx != -1 && !visited.contains(neighborIdx) &&
                    nodes.get(neighborIdx).active && nodes.get(neighborIdx).color == color) {
                    
                    visited.add(neighborIdx);
                    q.add(neighborIdx);
                    cluster.add(new Pair<>(nodes.get(neighborIdx).row, nodes.get(neighborIdx).col));
                }
            }
        }
        
        return cluster;
    }
    
    private void applyGravity() {
        // Vertical gravity: tiles fall down
        for (int j = 0; j < cols; j++) {
            int pos = rows - 1;
            for (int i = rows - 1; i >= 0; i--) {
                int nodeIdx = getNodeIndex(i, j);
                if (nodeIdx != -1 && nodes.get(nodeIdx).active) {
                    if (i != pos) {
                        // Move node to new position
                        Node node = nodes.get(nodeIdx);
                        
                        // Update position mapping
                        posToNodeIndex.get(i).remove(j);
                        posToNodeIndex.get(pos).put(j, nodeIdx);
                        
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
            boolean hasTiles = false;
            for (int i = 0; i < rows; i++) {
                int nodeIdx = getNodeIndex(i, j);
                if (nodeIdx != -1 && nodes.get(nodeIdx).active) {
                    hasTiles = true;
                    break;
                }
            }
            
            if (hasTiles) {
                if (j != col) {
                    for (int i = 0; i < rows; i++) {
                        int nodeIdx = getNodeIndex(i, j);
                        if (nodeIdx != -1) {
                            Node node = nodes.get(nodeIdx);
                            
                            // Update position mapping
                            posToNodeIndex.get(i).remove(j);
                            posToNodeIndex.get(i).put(col, nodeIdx);
                            
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
    
    // Helper classes
    public static class Pair<F, S> {
        private final F first;
        private final S second;
        
        public Pair(F first, S second) {
            this.first = first;
            this.second = second;
        }
        
        public F getFirst() { return first; }
        public S getSecond() { return second; }
    }
    
    public static class ClusterInfo {
        public int size;
        public char color;
        public int row;
        public int col;
        
        public ClusterInfo(int size, char color, int row, int col) {
            this.size = size;
            this.color = color;
            this.row = row;
            this.col = col;
        }
    }
}
