#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>

// Using the entire std namespace for simplicity in an interview context.
using namespace std;

// Enum to represent the piece types for clarity and type safety.
enum class PieceType {
    X,
    O,
    EMPTY
};

// Player class to hold player-specific data.
class Player {
private:
    string name;
    PieceType pieceType;

public:
    Player(string name, PieceType pieceType) : name(name), pieceType(pieceType) {}

    string getName() const {
        return name;
    }

    PieceType getPieceType() const {
        return pieceType;
    }
};

// Board class to manage the game grid and state.
class Board {
private:
    int size;
    vector<vector<PieceType>> grid;

public:
    Board(int size) : size(size) {
        grid.resize(size, vector<PieceType>(size, PieceType::EMPTY));
    }

    // Tries to add a piece to the board. Returns false if the move is invalid.
    bool addPiece(int row, int col, PieceType pieceType) {
        if (row >= 0 && row < size && col >= 0 && col < size && grid[row][col] == PieceType::EMPTY) {
            grid[row][col] = pieceType;
            return true;
        }
        return false;
    }

    // Checks if there are any free cells left on the board.
    bool hasFreeCells() {
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                if (grid[i][j] == PieceType::EMPTY) {
                    return true;
                }
            }
        }
        return false;
    }

    // Checks if the player with the given pieceType has won.
    bool checkWinner(PieceType pieceType) {
        // Check rows and columns
        for (int i = 0; i < size; ++i) {
            bool rowWin = true;
            bool colWin = true;
            for (int j = 0; j < size; ++j) {
                if (grid[i][j] != pieceType) rowWin = false;
                if (grid[j][i] != pieceType) colWin = false;
            }
            if (rowWin || colWin) return true;
        }

        // Check diagonals
        bool mainDiagWin = true;
        bool antiDiagWin = true;
        for (int i = 0; i < size; ++i) {
            if (grid[i][i] != pieceType) mainDiagWin = false;
            if (grid[i][size - 1 - i] != pieceType) antiDiagWin = false;
        }
        if (mainDiagWin || antiDiagWin) return true;

        return false;
    }
    
    // Prints the current state of the board to the console.
    void printBoard() {
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                if (grid[i][j] == PieceType::EMPTY) {
                    cout << "  |";
                } else {
                    cout << (grid[i][j] == PieceType::X ? " X|" : " O|");
                }
            }
            cout << endl;
        }
        cout << "-----------------" << endl;
    }
};

// Game class to orchestrate the entire game flow.
class Game {
private:
    Board* board;
    vector<Player*> players;
    int currentPlayerIndex;

public:
    Game(int boardSize, Player* player1, Player* player2) {
        board = new Board(boardSize);
        players.push_back(player1);
        players.push_back(player2);
        currentPlayerIndex = 0;
    }

    ~Game() {
        delete board;
        for (Player* p : players) {
            delete p;
        }
    }

    void startGame() {
        cout << "--- Tic-Tac-Toe Game Started ---" << endl;
        board->printBoard();
        
        while (true) {
            Player* currentPlayer = players[currentPlayerIndex];
            cout << currentPlayer->getName() << "'s turn. Enter row and column (0-2): ";

            int row, col;
            cin >> row >> col;

            // Add the piece to the board
            if (!board->addPiece(row, col, currentPlayer->getPieceType())) {
                cout << "Invalid move! That cell is already taken or out of bounds. Try again." << endl;
                continue;
            }

            board->printBoard();

            // Check for a winner
            if (board->checkWinner(currentPlayer->getPieceType())) {
                cout << "Congratulations " << currentPlayer->getName() << "! You have won!" << endl;
                break;
            }
            
            // Check for a draw
            if (!board->hasFreeCells()) {
                cout << "The game is a draw!" << endl;
                break;
            }

            // Switch to the next player
            currentPlayerIndex = 1 - currentPlayerIndex;
        }
    }
};

int main() {
    // Initialize players
    Player* player1 = new Player("Player 1", PieceType::X);
    Player* player2 = new Player("Player 2", PieceType::O);

    // Create and start the game
    Game ticTacToeGame(3, player1, player2);
    ticTacToeGame.startGame();

    return 0;
}
