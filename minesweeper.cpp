#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

// Represents a single cell on the board
class Cell {
private:
    bool isMine;
    bool isRevealed;
    int adjacentMines;

public:
    Cell() : isMine(false), isRevealed(false), adjacentMines(0) {}

    bool getIsMine() const { return isMine; }
    void setIsMine(bool mine) { isMine = mine; }

    bool getIsRevealed() const { return isRevealed; }
    void reveal() { isRevealed = true; }

    int getAdjacentMines() const { return adjacentMines; }
    void setAdjacentMines(int count) { adjacentMines = count; }

    // Character to display for this cell based on its state
    char getDisplayChar() const {
        if (!isRevealed) {
            return '#'; // Hidden
        }
        if (isMine) {
            return '*'; // Mine
        }
        if (adjacentMines == 0) {
            return ' '; // Empty
        }
        return adjacentMines + '0'; // Number
    }
};


// Represents the game board containing a grid of cells
class Board {
private:
    vector<vector<Cell>> grid;
    int rows;
    int cols;
    int numMines;

    // Helper function to check if coordinates are valid
    bool isValid(int r, int c) {
        return r >= 0 && r < rows && c >= 0 && c < cols;
    }

    // Places mines randomly on the grid
    void placeMines() {
        int minesPlaced = 0;
        while (minesPlaced < numMines) {
            int r = rand() % rows;
            int c = rand() % cols;
            if (!grid[r][c].getIsMine()) {
                grid[r][c].setIsMine(true);
                minesPlaced++;
            }
        }
    }

    // Calculates the number of adjacent mines for each cell
    void calculateAdjacentMines() {
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                if (grid[r][c].getIsMine()) continue;

                int count = 0;
                // Check all 8 neighbors
                for (int dr = -1; dr <= 1; ++dr) {
                    for (int dc = -1; dc <= 1; ++dc) {
                        if (dr == 0 && dc == 0) continue;
                        int nr = r + dr;
                        int nc = c + dc;
                        if (isValid(nr, nc) && grid[nr][nc].getIsMine()) {
                            count++;
                        }
                    }
                }
                grid[r][c].setAdjacentMines(count);
            }
        }
    }

public:
    Board(int r, int c, int mines) : rows(r), cols(c), numMines(mines) {
        grid.resize(rows, vector<Cell>(cols));
        placeMines();
        calculateAdjacentMines();
    }

    void displayBoard(bool showMines = false) const {
        cout << "   ";
        for (int c = 0; c < cols; ++c) {
            cout << c << " ";
        }
        cout << endl << "  --";
        for (int c = 0; c < cols; ++c) {
            cout << "--";
        }
        cout << endl;

        for (int r = 0; r < rows; ++r) {
            cout << r << " |";
            for (int c = 0; c < cols; ++c) {
                Cell temp = grid[r][c];
                if (showMines) temp.reveal();
                cout << temp.getDisplayChar() << " ";
            }
            cout << "|" << endl;
        }
         cout << "  --";
        for (int c = 0; c < cols; ++c) {
            cout << "--";
        }
        cout << endl;
    }

    // Recursively reveal cells starting from (r, c)
    bool revealCell(int r, int c) {
        if (!isValid(r, c) || grid[r][c].getIsRevealed()) {
            return true; // Already revealed or invalid, not a losing move
        }
        
        grid[r][c].reveal();

        if (grid[r][c].getIsMine()) {
            return false; // Hit a mine!
        }

        // If cell is empty (0 adjacent mines), reveal its neighbors
        if (grid[r][c].getAdjacentMines() == 0) {
            for (int dr = -1; dr <= 1; ++dr) {
                for (int dc = -1; dc <= 1; ++dc) {
                    if (dr == 0 && dc == 0) continue;
                    revealCell(r + dr, c + dc);
                }
            }
        }
        return true; // Safe move
    }
    
    // Check if the player has won the game
    bool checkWin() {
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                if (!grid[r][c].getIsMine() && !grid[r][c].getIsRevealed()) {
                    return false; // Found a non-mine cell that is not yet revealed
                }
            }
        }
        return true; // All non-mine cells are revealed
    }
};

// Singleton Game class to manage the game flow
class Game {
private:
    static Game* instance;
    Board* board;
    bool isGameOver;
    int rows, cols, mines;

    // Private constructor for Singleton
    Game(int r, int c, int m) : rows(r), cols(c), mines(m) {
        board = new Board(rows, cols, mines);
        isGameOver = false;
    }

    // Clean up memory
    ~Game() {
        delete board;
        instance = nullptr;
    }

public:
    // Delete copy constructor and assignment operator
    Game(const Game&) = delete;
    void operator=(const Game&) = delete;

    // Static method to get the single instance
    static Game* getInstance(int r, int c, int m) {
        if (instance == nullptr) {
            instance = new Game(r, c, m);
        }
        return instance;
    }
    
    static void destroyInstance() {
        delete instance;
    }

    void run() {
        while (!isGameOver) {
            board->displayBoard();
            cout << "Enter row and column to reveal: ";
            int r, c;
            cin >> r >> c;

            if (!board->revealCell(r, c)) {
                cout << "\nBOOM! You hit a mine. Game Over." << endl;
                isGameOver = true;
                board->displayBoard(true); // Show all mines
            } else {
                if (board->checkWin()) {
                    cout << "\nCongratulations! You have cleared all the mines!" << endl;
                    isGameOver = true;
                    board->displayBoard(true);
                }
            }
        }
    }
};

// Initialize static instance to nullptr
Game* Game::instance = nullptr;


// Client code
int main() {
    srand(time(0)); // Seed for random number generation
    
    const int ROWS = 9;
    const int COLS = 9;
    const int MINES = 10;

    cout << "--- Welcome to Minesweeper! ---" << endl;
    
    Game* minesweeper = Game::getInstance(ROWS, COLS, MINES);
    minesweeper->run();
    Game::destroyInstance(); // Clean up the singleton instance

    return 0;
}
