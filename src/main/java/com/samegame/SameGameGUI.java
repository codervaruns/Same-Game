package com.samegame;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.util.List;
import java.util.Random;

public class SameGameGUI extends JFrame {
    private static final int WINDOW_WIDTH = 750;
    private static final int WINDOW_HEIGHT = 650;
    private static final int TILE_SIZE = 80;
    private static final int GRID_OFFSET_X = 80;
    private static final int GRID_OFFSET_Y = 140;
    private static final int UI_HEIGHT = 120;
    
    private SameGame game;
    private GamePanel gamePanel;
    
    // Mouse state
    private int hoveredRow = -1;
    private int hoveredCol = -1;
    private List<SameGame.Pair<Integer, Integer>> hoveredCluster = null;
    
    // Game state
    private boolean gameOver = false;
    private boolean gameWon = false;
    
    // Computer AI timing
    private long lastComputerMoveTime = 0;
    private static final long COMPUTER_MOVE_DELAY = 3000;  // 3 second delay for computer move
    private Timer gameTimer;
    
    public SameGameGUI(SameGame game) {
        this.game = game;
        
        setTitle("Same Game");
        setSize(WINDOW_WIDTH, WINDOW_HEIGHT);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setResizable(false);
        
        gamePanel = new GamePanel();
        add(gamePanel);
        
        // Setup key listeners on the panel (which can receive focus)
        gamePanel.setFocusable(true);
        gamePanel.addKeyListener(new KeyAdapter() {
            @Override
            public void keyPressed(KeyEvent e) {
                if (e.getKeyCode() == KeyEvent.VK_R) {
                    handleRestart();
                } else if (e.getKeyCode() == KeyEvent.VK_ESCAPE) {
                    System.exit(0);
                }
            }
        });
        gamePanel.requestFocusInWindow();
        
        // Setup game timer for computer moves
        gameTimer = new Timer(16, e -> {
            // Execute computer move if it's computer's turn and enough time has passed
            if (!gameOver && !game.getUserTurn()) {
                long currentTime = System.currentTimeMillis();
                if (currentTime - lastComputerMoveTime >= COMPUTER_MOVE_DELAY) {
                    executeComputerMove();
                    lastComputerMoveTime = currentTime;
                }
            }
            gamePanel.repaint();
        });
        gameTimer.start();
        
        lastComputerMoveTime = System.currentTimeMillis();
    }
    
    private Color getColorForTile(char tile) {
        switch(tile) {
            case 'G': return new Color(0, 200, 0);      // Green
            case 'W': return new Color(255, 255, 255);  // White
            case 'R': return new Color(200, 0, 0);      // Red
            case 'B': return new Color(0, 0, 200);      // Blue
            case 'Y': return new Color(200, 200, 0);    // Yellow
            default: return new Color(128, 128, 128);   // Gray
        }
    }
    
    private void handleMouseMove(int mouseX, int mouseY) {
        int col = (mouseX - GRID_OFFSET_X) / TILE_SIZE;
        int row = (mouseY - GRID_OFFSET_Y) / TILE_SIZE;
        
        if (row >= 0 && row < game.getRows() && col >= 0 && col < game.getCols()) {
            if (row != hoveredRow || col != hoveredCol) {
                hoveredRow = row;
                hoveredCol = col;
                
                if (row >= 0 && col >= 0 && game.isTileActive(row, col)) {
                    hoveredCluster = game.getCluster(row, col);
                } else {
                    hoveredCluster = null;
                }
            }
        } else {
            hoveredRow = -1;
            hoveredCol = -1;
            hoveredCluster = null;
        }
    }
    
    private void handleMouseClick(int mouseX, int mouseY) {
        if (gameOver || !game.getUserTurn()) return;  // Only allow clicks on user's turn
        
        int col = (mouseX - GRID_OFFSET_X) / TILE_SIZE;
        int row = (mouseY - GRID_OFFSET_Y) / TILE_SIZE;
        
        if (row >= 0 && row < game.getRows() && col >= 0 && col < game.getCols()) {
            if (game.removeCluster(row, col)) {
                // Clear hovered state after move
                hoveredCluster = null;
                
                // Check if game is over
                if (!game.hasMovesLeft()) {
                    gameOver = true;
                    // Check if won (no tiles left or very few)
                    int activeTiles = 0;
                    for (int i = 0; i < game.getRows(); i++) {
                        for (int j = 0; j < game.getCols(); j++) {
                            if (game.isTileActive(i, j)) activeTiles++;
                        }
                    }
                    gameWon = (activeTiles == 0);
                }
            }
        }
    }
    
    private void handleRestart() {
        char[][] initialGrid = generateRandomGrid(6, 8);
        game.reset(initialGrid);
        gameOver = false;
        gameWon = false;
        hoveredCluster = null;
        lastComputerMoveTime = System.currentTimeMillis();
    }
    
    private void executeComputerMove() {
        if (gameOver || game.getUserTurn()) return;
        
        // Get the best move using greedy algorithm
        SameGame.Pair<Integer, Integer> bestMove = game.getBestMove();
        
        if (bestMove.getFirst() != -1 && bestMove.getSecond() != -1) {
            // Execute the move
            game.removeCluster(bestMove.getFirst(), bestMove.getSecond());
            
            // Check if game is over
            if (!game.hasMovesLeft()) {
                gameOver = true;
                int activeTiles = 0;
                for (int i = 0; i < game.getRows(); i++) {
                    for (int j = 0; j < game.getCols(); j++) {
                        if (game.isTileActive(i, j)) activeTiles++;
                    }
                }
                gameWon = (activeTiles == 0);
            }
        }
    }
    
    private static char[][] generateRandomGrid(int rows, int cols) {
        char[][] grid = new char[rows][cols];
        char[] colors = {'G', 'W', 'R', 'B', 'Y'};  // Green, White, Red, Blue, Yellow
        
        Random rand = new Random();
        
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                grid[i][j] = colors[rand.nextInt(colors.length)];
            }
        }
        
        return grid;
    }
    
    class GamePanel extends JPanel {
        public GamePanel() {
            setBackground(new Color(20, 20, 20));
            
            addMouseMotionListener(new MouseMotionAdapter() {
                @Override
                public void mouseMoved(MouseEvent e) {
                    handleMouseMove(e.getX(), e.getY());
                    repaint();
                }
            });
            
            addMouseListener(new MouseAdapter() {
                @Override
                public void mouseClicked(MouseEvent e) {
                    if (e.getButton() == MouseEvent.BUTTON1) {
                        handleMouseClick(e.getX(), e.getY());
                        repaint();
                    }
                }
            });
        }
        
        @Override
        protected void paintComponent(Graphics g) {
            super.paintComponent(g);
            Graphics2D g2d = (Graphics2D) g;
            g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
            
            drawGrid(g2d);
            drawUI(g2d);
        }
        
        private void drawGrid(Graphics2D g) {
            // Draw background
            g.setColor(new Color(40, 40, 40));
            g.fillRect(GRID_OFFSET_X - 5, GRID_OFFSET_Y - 5, 
                      game.getCols() * TILE_SIZE + 10, 
                      game.getRows() * TILE_SIZE + 10);
            
            // Draw tiles
            for (int i = 0; i < game.getRows(); i++) {
                for (int j = 0; j < game.getCols(); j++) {
                    boolean highlight = false;
                    
                    // Check if this tile is in the hovered cluster
                    if (hoveredCluster != null) {
                        for (SameGame.Pair<Integer, Integer> tile : hoveredCluster) {
                            if (tile.getFirst() == i && tile.getSecond() == j) {
                                highlight = true;
                                break;
                            }
                        }
                    }
                    
                    drawTile(g, i, j, highlight);
                }
            }
        }
        
        private void drawTile(Graphics2D g, int row, int col, boolean highlight) {
            if (!game.isTileActive(row, col)) return;
            
            int x = GRID_OFFSET_X + col * TILE_SIZE;
            int y = GRID_OFFSET_Y + row * TILE_SIZE;
            
            Color color = getColorForTile(game.getTile(row, col));
            
            // Draw tile background
            g.setColor(color);
            g.fillRect(x + 2, y + 2, TILE_SIZE - 4, TILE_SIZE - 4);
            
            // Draw highlight border if needed
            if (highlight) {
                g.setColor(new Color(255, 255, 0));
                for (int i = 0; i < 4; i++) {
                    g.drawRect(x + i, y + i, TILE_SIZE - 2*i, TILE_SIZE - 2*i);
                }
            } else {
                // Draw normal border
                g.setColor(new Color(64, 64, 64));
                g.drawRect(x, y, TILE_SIZE, TILE_SIZE);
            }
        }
        
        private void drawUI(Graphics2D g) {
            g.setFont(new Font("Arial", Font.BOLD, 24));
            
            // Draw turn indicator
            String turnText = game.getUserTurn() ? "YOUR TURN" : "COMPUTER'S TURN";
            Color turnColor = game.getUserTurn() ? new Color(100, 255, 100) : new Color(255, 100, 100);
            g.setColor(turnColor);
            g.drawString(turnText, 20, 40);
            
            // Draw user score
            g.setColor(new Color(100, 200, 255));
            g.drawString("User Score: " + game.getUserScore(), 20, 70);
            
            // Draw computer score
            g.setColor(new Color(255, 150, 100));
            g.drawString("Computer Score: " + game.getComputerScore(), 20, 100);
            
            // Draw total moves
            g.setColor(new Color(200, 200, 200));
            g.drawString("Total Moves: " + game.getMoves(), 300, 70);
            
            // Draw cluster size if hovering
            if (hoveredCluster != null && hoveredCluster.size() >= 2) {
                int clusterSize = hoveredCluster.size();
                int points = (clusterSize - 2) * (clusterSize - 2);
                String clusterText = "Cluster: " + clusterSize + " tiles (" + points + " pts)";
                g.setColor(new Color(255, 255, 0));
                g.drawString(clusterText, 250, 40);
            }
            
            // Show "Thinking..." when computer is about to move
            if (!gameOver && !game.getUserTurn()) {
                g.setColor(new Color(255, 200, 100));
                g.drawString("Thinking...", 250, 70);
            }
            
            // Draw game over message
            if (gameOver) {
                String message;
                if (gameWon) {
                    message = "All tiles cleared!";
                } else if (game.getUserScore() > game.getComputerScore()) {
                    message = "USER WINS! " + game.getUserScore() + " - " + game.getComputerScore();
                } else if (game.getComputerScore() > game.getUserScore()) {
                    message = "COMPUTER WINS! " + game.getComputerScore() + " - " + game.getUserScore();
                } else {
                    message = "TIE GAME! " + game.getUserScore() + " - " + game.getComputerScore();
                }
                
                g.setFont(new Font("Arial", Font.BOLD, 28));
                g.setColor(new Color(255, 100, 100));
                FontMetrics fm = g.getFontMetrics();
                int msgWidth = fm.stringWidth(message);
                g.drawString(message, WINDOW_WIDTH / 2 - msgWidth / 2, WINDOW_HEIGHT - 70);
                
                String restartText = "Press R to Restart";
                g.setFont(new Font("Arial", Font.PLAIN, 20));
                g.setColor(new Color(200, 200, 200));
                fm = g.getFontMetrics();
                int restartWidth = fm.stringWidth(restartText);
                g.drawString(restartText, WINDOW_WIDTH / 2 - restartWidth / 2, WINDOW_HEIGHT - 40);
            }
        }
    }
    
    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> {
            // Initialize game with random grid
            char[][] initialGrid = generateRandomGrid(6, 8);
            
            SameGame game = new SameGame(initialGrid);
            SameGameGUI gui = new SameGameGUI(game);
            
            gui.setVisible(true);
        });
    }
}
