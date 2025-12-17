#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "SameGame.h"
#include <iostream>
#include <string>
#include <cmath>

using namespace std;

// Function to generate random grid
vector<vector<char>> generateRandomGrid(int rows, int cols) {
    vector<vector<char>> grid(rows, vector<char>(cols));
    vector<char> colors = {'G', 'W', 'R', 'B', 'Y'};  // Green, White, Red, Blue, Yellow
    
    srand(time(0));
    
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            grid[i][j] = colors[rand() % colors.size()];
        }
    }
    
    return grid;
}

const int WINDOW_WIDTH = 750;
const int WINDOW_HEIGHT = 650;
const int TILE_SIZE = 80;
const int GRID_OFFSET_X = 80;
const int GRID_OFFSET_Y = 140;
const int UI_HEIGHT = 120;

class SameGameGUI {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
    SameGame* game;
    
    // Mouse state
    int hoveredRow;
    int hoveredCol;
    vector<pair<int, int>> hoveredCluster;
    
    // Game state
    bool gameOver;
    bool gameWon;
    
    // Computer AI timing
    Uint32 lastComputerMoveTime;
    const Uint32 COMPUTER_MOVE_DELAY = 3000;  // 3 second delay for computer move
    
    // Colors
    SDL_Color getColorForTile(char tile) {
        switch(tile) {
            case 'G': return {0, 200, 0, 255};      // Green
            case 'W': return {255, 255, 255, 255};  // White
            case 'R': return {200, 0, 0, 255};      // Red
            case 'B': return {0, 0, 200, 255};      // Blue
            case 'Y': return {200, 200, 0, 255};    // Yellow
            default: return {128, 128, 128, 255};   // Gray
        }
    }
    
    void drawTile(int row, int col, bool highlight = false) {
        if (!game->isTileActive(row, col)) return;
        
        int x = GRID_OFFSET_X + col * TILE_SIZE;
        int y = GRID_OFFSET_Y + row * TILE_SIZE;
        
        SDL_Color color = getColorForTile(game->getTile(row, col));
        
        // Draw tile background
        SDL_Rect tileRect = {x + 2, y + 2, TILE_SIZE - 4, TILE_SIZE - 4};
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(renderer, &tileRect);
        
        // Draw highlight border if needed
        if (highlight) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            for(int i = 0; i < 4; i++) {
                SDL_Rect borderRect = {x + i, y + i, TILE_SIZE - 2*i, TILE_SIZE - 2*i};
                SDL_RenderDrawRect(renderer, &borderRect);
            }
        } else {
            // Draw normal border
            SDL_SetRenderDrawColor(renderer, 64, 64, 64, 255);
            SDL_Rect borderRect = {x, y, TILE_SIZE, TILE_SIZE};
            SDL_RenderDrawRect(renderer, &borderRect);
        }
    }
    
    void drawGrid() {
        // Draw background
        SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
        SDL_Rect bgRect = {GRID_OFFSET_X - 5, GRID_OFFSET_Y - 5, 
                          game->getCols() * TILE_SIZE + 10, 
                          game->getRows() * TILE_SIZE + 10};
        SDL_RenderFillRect(renderer, &bgRect);
        
        // Draw tiles
        for (int i = 0; i < game->getRows(); i++) {
            for (int j = 0; j < game->getCols(); j++) {
                bool highlight = false;
                
                // Check if this tile is in the hovered cluster
                for (const auto& tile : hoveredCluster) {
                    if (tile.first == i && tile.second == j) {
                        highlight = true;
                        break;
                    }
                }
                
                drawTile(i, j, highlight);
            }
        }
    }
    
    void drawUI() {
        // Draw turn indicator
        string turnText = game->getUserTurn() ? "YOUR TURN" : "COMPUTER'S TURN";
        SDL_Color turnColor = game->getUserTurn() ? SDL_Color{100, 255, 100, 255} : SDL_Color{255, 100, 100, 255};
        renderText(turnText, 20, 20, turnColor);
        
        // Draw user score
        string userScoreText = "User Score: " + to_string(game->getUserScore());
        renderText(userScoreText, 20, 50, {100, 200, 255, 255});
        
        // Draw computer score
        string computerScoreText = "Computer Score: " + to_string(game->getComputerScore());
        renderText(computerScoreText, 20, 80, {255, 150, 100, 255});
        
        // Draw total moves
        string movesText = "Total Moves: " + to_string(game->getMoves());
        renderText(movesText, 300, 50, {200, 200, 200, 255});
        
        // Draw cluster size if hovering
        if (!hoveredCluster.empty() && hoveredCluster.size() >= 2) {
            int clusterSize = hoveredCluster.size();
            int points = (clusterSize - 2) * (clusterSize - 2);
            string clusterText = "Cluster: " + to_string(clusterSize) + " tiles (" + to_string(points) + " pts)";
            renderText(clusterText, 250, 20, {255, 255, 0, 255});
        }
        
        // Show "Thinking..." when computer is about to move
        if (!gameOver && !game->getUserTurn()) {
            renderText("Thinking...", 250, 50, {255, 200, 100, 255});
        }
        
        // Draw game over message
        if (gameOver) {
            string message;
            if (gameWon) {
                message = "All tiles cleared!";
            } else if (game->getUserScore() > game->getComputerScore()) {
                message = "USER WINS! " + to_string(game->getUserScore()) + " - " + to_string(game->getComputerScore());
            } else if (game->getComputerScore() > game->getUserScore()) {
                message = "COMPUTER WINS! " + to_string(game->getComputerScore()) + " - " + to_string(game->getUserScore());
            } else {
                message = "TIE GAME! " + to_string(game->getUserScore()) + " - " + to_string(game->getComputerScore());
            }
            renderText(message, WINDOW_WIDTH / 2 - 200, WINDOW_HEIGHT - 50, {255, 100, 100, 255}, true);
            renderText("Press R to Restart", WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT - 20, {200, 200, 200, 255}, true);
        }
    }
    
    void renderText(const string& text, int x, int y, SDL_Color color, bool large = false) {
        if (!font) return;
        
        SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
        if (!surface) return;
        
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture) {
            SDL_FreeSurface(surface);
            return;
        }
        
        SDL_Rect destRect = {x, y, surface->w, surface->h};
        SDL_RenderCopy(renderer, texture, nullptr, &destRect);
        
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(surface);
    }
    
    pair<int, int> screenToGrid(int mouseX, int mouseY) {
        int col = (mouseX - GRID_OFFSET_X) / TILE_SIZE;
        int row = (mouseY - GRID_OFFSET_Y) / TILE_SIZE;
        
        if (row >= 0 && row < game->getRows() && col >= 0 && col < game->getCols()) {
            return {row, col};
        }
        return {-1, -1};
    }
    
public:
    SameGameGUI(SameGame* gameInstance) : 
        window(nullptr), renderer(nullptr), font(nullptr),
        game(gameInstance), hoveredRow(-1), hoveredCol(-1),
        gameOver(false), gameWon(false), lastComputerMoveTime(0) {}
    
    ~SameGameGUI() {
        cleanup();
    }
    
    bool init() {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
            return false;
        }
        
        if (TTF_Init() < 0) {
            cerr << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << endl;
            return false;
        }
        
        window = SDL_CreateWindow("Same Game", 
                                  SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
        if (!window) {
            cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
            return false;
        }
        
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer) {
            cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << endl;
            return false;
        }
        
        // Try to load font (use default if not found)
        font = TTF_OpenFont("C:\\Windows\\Fonts\\arial.ttf", 32);
        if (!font) {
            cerr << "Warning: Could not load font! Using default rendering." << endl;
        }
        
        return true;
    }
    
    void cleanup() {
        if (font) {
            TTF_CloseFont(font);
            font = nullptr;
        }
        if (renderer) {
            SDL_DestroyRenderer(renderer);
            renderer = nullptr;
        }
        if (window) {
            SDL_DestroyWindow(window);
            window = nullptr;
        }
        TTF_Quit();
        SDL_Quit();
    }
    
    void handleMouseMove(int mouseX, int mouseY) {
        pair<int, int> pos = screenToGrid(mouseX, mouseY);
        int row = pos.first;
        int col = pos.second;
        
        if (row != hoveredRow || col != hoveredCol) {
            hoveredRow = row;
            hoveredCol = col;
            
            if (row >= 0 && col >= 0 && game->isTileActive(row, col)) {
                hoveredCluster = game->getCluster(row, col);
            } else {
                hoveredCluster.clear();
            }
        }
    }
    
    void handleMouseClick(int mouseX, int mouseY) {
        if (gameOver || !game->getUserTurn()) return;  // Only allow clicks on user's turn
        
        pair<int, int> pos = screenToGrid(mouseX, mouseY);
        int row = pos.first;
        int col = pos.second;
        
        if (row >= 0 && col >= 0) {
            if (game->removeCluster(row, col)) {
                // Clear hovered state after move
                hoveredCluster.clear();
                
                // Check if game is over
                if (!game->hasMovesLeft()) {
                    gameOver = true;
                    // Check if won (no tiles left or very few)
                    int activeTiles = 0;
                    for (int i = 0; i < game->getRows(); i++) {
                        for (int j = 0; j < game->getCols(); j++) {
                            if (game->isTileActive(i, j)) activeTiles++;
                        }
                    }
                    gameWon = (activeTiles == 0);
                }
            }
        }
    }
    
    void handleRestart() {
        vector<vector<char>> initialGrid = generateRandomGrid(6, 8);
        game->reset(initialGrid);
        gameOver = false;
        gameWon = false;
        hoveredCluster.clear();
        lastComputerMoveTime = SDL_GetTicks();
    }
    
    void executeComputerMove() {
        if (gameOver || game->getUserTurn()) return;
        
        // Get the best move using greedy algorithm
        pair<int, int> bestMove = game->getBestMove();
        
        if (bestMove.first != -1 && bestMove.second != -1) {
            // Execute the move
            game->removeCluster(bestMove.first, bestMove.second);
            
            // Check if game is over
            if (!game->hasMovesLeft()) {
                gameOver = true;
                int activeTiles = 0;
                for (int i = 0; i < game->getRows(); i++) {
                    for (int j = 0; j < game->getCols(); j++) {
                        if (game->isTileActive(i, j)) activeTiles++;
                    }
                }
                gameWon = (activeTiles == 0);
            }
        }
    }
    
    void render() {
        // Clear screen
        SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
        SDL_RenderClear(renderer);
        
        // Draw game elements
        drawGrid();
        drawUI();
        
        // Present
        SDL_RenderPresent(renderer);
    }
    
    void run() {
        bool quit = false;
        SDL_Event e;
        
        while (!quit) {
            while (SDL_PollEvent(&e) != 0) {
                if (e.type == SDL_QUIT) {
                    quit = true;
                } else if (e.type == SDL_MOUSEMOTION) {
                    handleMouseMove(e.motion.x, e.motion.y);
                } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                    if (e.button.button == SDL_BUTTON_LEFT) {
                        handleMouseClick(e.button.x, e.button.y);
                    }
                } else if (e.type == SDL_KEYDOWN) {
                    if (e.key.keysym.sym == SDLK_r) {
                        handleRestart();
                    } else if (e.key.keysym.sym == SDLK_ESCAPE) {
                        quit = true;
                    }
                }
            }
            
            // Execute computer move if it's computer's turn and enough time has passed
            if (!gameOver && !game->getUserTurn()) {
                Uint32 currentTime = SDL_GetTicks();
                if (currentTime - lastComputerMoveTime >= COMPUTER_MOVE_DELAY) {
                    executeComputerMove();
                    lastComputerMoveTime = currentTime;
                }
            }
            
            render();
            SDL_Delay(16); // ~60 FPS
        }
    }
};

int main(int argc, char* argv[]) {
    // Initialize game with random grid
    vector<vector<char>> initialGrid = generateRandomGrid(6, 8);
    
    SameGame game(initialGrid);
    SameGameGUI gui(&game);
    
    if (!gui.init()) {
        cerr << "Failed to initialize GUI!" << endl;
        return 1;
    }
    
    gui.run();
    
    return 0;
}
