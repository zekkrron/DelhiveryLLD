#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <cmath>

using namespace std;

// Forward declarations to resolve circular dependencies
class Piece;
class Board;

// Enums for clarity
enum class Color { WHITE, BLACK };
enum class PieceType { KING, QUEEN, ROOK, BISHOP, KNIGHT, PAWN };

// Represents a single square on the board
class Spot {
private:
    int x, y;
    Piece* piece;

public:
    Spot(int x, int y, Piece* piece) : x(x), y(y), piece(piece) {}

    Piece* getPiece() const {
        return piece;
    }

    void setPiece(Piece* p) {
        piece = p;
    }

    int getX() const {
        return x;
    }

    int getY() const {
        return y;
    }
};

// Represents the 8x8 game board
class Board {
private:
    vector<vector<Spot*>> boxes;

public:
    Board() {
        boxes.resize(8, vector<Spot*>(8, nullptr));
        resetBoard();
    }

    Spot* getBox(int x, int y) const {
        if (x < 0 || x >= 8 || y < 0 || y >= 8) {
            throw out_of_range("Index out of bounds");
        }
        return boxes[x][y];
    }

    void resetBoard(); // Implementation after Piece is defined
};

// Abstract base class for all pieces
class Piece {
protected:
    Color color;
    PieceType type;

public:
    Piece(Color color, PieceType type) : color(color), type(type) {}
    virtual ~Piece() = default;

    Color getColor() const {
        return color;
    }

    PieceType getType() const {
        return type;
    }

    // The core of the Strategy Pattern: each piece implements its own move logic
    virtual bool canMove(const Board& board, const Spot& start, const Spot& end) const = 0;
    
    // Static Factory Method
    static Piece* createPiece(PieceType type, Color color);
};


// Concrete Piece Implementations
class Pawn : public Piece {
public:
    Pawn(Color color) : Piece(color, PieceType::PAWN) {}
    bool canMove(const Board& board, const Spot& start, const Spot& end) const override {
        // Basic pawn move: one step forward
        // Does not include initial two-step move, en passant, or capturing diagonally
        int forward = (getColor() == Color::WHITE) ? 1 : -1;
        if (start.getX() + forward == end.getX() && start.getY() == end.getY() && end.getPiece() == nullptr) {
            return true;
        }
        return false;
    }
};

class Knight : public Piece {
public:
    Knight(Color color) : Piece(color, PieceType::KNIGHT) {}
    bool canMove(const Board& board, const Spot& start, const Spot& end) const override {
        int dx = abs(start.getX() - end.getX());
        int dy = abs(start.getY() - end.getY());
        return (dx == 2 && dy == 1) || (dx == 1 && dy == 2);
    }
};

class Bishop : public Piece {
public:
    Bishop(Color color) : Piece(color, PieceType::BISHOP) {}
    bool canMove(const Board& board, const Spot& start, const Spot& end) const override {
        // Must be a diagonal move
        if (abs(start.getX() - end.getX()) != abs(start.getY() - end.getY())) {
            return false;
        }
        // Simplified: doesn't check for pieces in the way
        return true;
    }
};

class Rook : public Piece {
public:
    Rook(Color color) : Piece(color, PieceType::ROOK) {}
    bool canMove(const Board& board, const Spot& start, const Spot& end) const override {
        // Must be a straight line move
        if (start.getX() != end.getX() && start.getY() != end.getY()) {
            return false;
        }
        // Simplified: doesn't check for pieces in the way
        return true;
    }
};

class Queen : public Piece {
public:
    Queen(Color color) : Piece(color, PieceType::QUEEN) {}
    bool canMove(const Board& board, const Spot& start, const Spot& end) const override {
        // Can move like a rook or a bishop
        bool isStraight = (start.getX() == end.getX() || start.getY() == end.getY());
        bool isDiagonal = (abs(start.getX() - end.getX()) == abs(start.getY() - end.getY()));
        if (!isStraight && !isDiagonal) {
            return false;
        }
        // Simplified: doesn't check for pieces in the way
        return true;
    }
};

class King : public Piece {
public:
    King(Color color) : Piece(color, PieceType::KING) {}
    bool canMove(const Board& board, const Spot& start, const Spot& end) const override {
        int dx = abs(start.getX() - end.getX());
        int dy = abs(start.getY() - end.getY());
        return dx <= 1 && dy <= 1;
    }
};

// Now we can implement the factory and board reset
Piece* Piece::createPiece(PieceType type, Color color) {
    switch (type) {
        case PieceType::PAWN: return new Pawn(color);
        case PieceType::KNIGHT: return new Knight(color);
        case PieceType::BISHOP: return new Bishop(color);
        case PieceType::ROOK: return new Rook(color);
        case PieceType::QUEEN: return new Queen(color);
        case PieceType::KING: return new King(color);
        default: return nullptr;
    }
}

void Board::resetBoard() {
    // Clear existing pieces if any (important for re-game)
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (boxes[i][j] != nullptr && boxes[i][j]->getPiece() != nullptr) {
                delete boxes[i][j]->getPiece();
            }
            delete boxes[i][j];
        }
    }

    // White pieces
    boxes[0][0] = new Spot(0, 0, Piece::createPiece(PieceType::ROOK, Color::WHITE));
    boxes[0][1] = new Spot(0, 1, Piece::createPiece(PieceType::KNIGHT, Color::WHITE));
    boxes[0][2] = new Spot(0, 2, Piece::createPiece(PieceType::BISHOP, Color::WHITE));
    boxes[0][3] = new Spot(0, 3, Piece::createPiece(PieceType::QUEEN, Color::WHITE));
    boxes[0][4] = new Spot(0, 4, Piece::createPiece(PieceType::KING, Color::WHITE));
    boxes[0][5] = new Spot(0, 5, Piece::createPiece(PieceType::BISHOP, Color::WHITE));
    boxes[0][6] = new Spot(0, 6, Piece::createPiece(PieceType::KNIGHT, Color::WHITE));
    boxes[0][7] = new Spot(0, 7, Piece::createPiece(PieceType::ROOK, Color::WHITE));
    for (int j = 0; j < 8; ++j) {
        boxes[1][j] = new Spot(1, j, Piece::createPiece(PieceType::PAWN, Color::WHITE));
    }

    // Black pieces
    boxes[7][0] = new Spot(7, 0, Piece::createPiece(PieceType::ROOK, Color::BLACK));
    boxes[7][1] = new Spot(7, 1, Piece::createPiece(PieceType::KNIGHT, Color::BLACK));
    boxes[7][2] = new Spot(7, 2, Piece::createPiece(PieceType::BISHOP, Color::BLACK));
    boxes[7][3] = new Spot(7, 3, Piece::createPiece(PieceType::QUEEN, Color::BLACK));
    boxes[7][4] = new Spot(7, 4, Piece::createPiece(PieceType::KING, Color::BLACK));
    boxes[7][5] = new Spot(7, 5, Piece::createPiece(PieceType::BISHOP, Color::BLACK));
    boxes[7][6] = new Spot(7, 6, Piece::createPiece(PieceType::KNIGHT, Color::BLACK));
    boxes[7][7] = new Spot(7, 7, Piece::createPiece(PieceType::ROOK, Color::BLACK));
    for (int j = 0; j < 8; ++j) {
        boxes[6][j] = new Spot(6, j, Piece::createPiece(PieceType::PAWN, Color::BLACK));
    }
    
    // Empty spots
    for(int i = 2; i < 6; ++i) {
        for(int j = 0; j < 8; ++j) {
            boxes[i][j] = new Spot(i, j, nullptr);
        }
    }
}

class Player {
private:
    Color color;
public:
    Player(Color color) : color(color) {}
    Color getColor() const { return color; }
};

// Main Game Controller - Singleton Pattern
class Game {
private:
    Board board;
    Player player1;
    Player player2;
    Player* currentPlayer;

    // Private constructor for Singleton
    Game() : board(), player1(Color::WHITE), player2(Color::BLACK), currentPlayer(&player1) {}
    
    // Prevent copying
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

public:
    // Static method to get the single instance
    static Game& getInstance() {
        static Game instance; // Guaranteed to be created only once
        return instance;
    }
    
    Board& getBoard() {
        return board;
    }

    bool makeMove(int startX, int startY, int endX, int endY) {
        Spot* startBox = board.getBox(startX, startY);
        Spot* endBox = board.getBox(endX, endY);
        Piece* sourcePiece = startBox->getPiece();

        // 1. Basic validation
        if (sourcePiece == nullptr) {
            cout << "No piece at starting position." << endl;
            return false;
        }

        if (sourcePiece->getColor() != currentPlayer->getColor()) {
            cout << "Not your turn." << endl;
            return false;
        }

        // 2. Check if the destination has a piece of the same color
        if (endBox->getPiece() != nullptr && endBox->getPiece()->getColor() == currentPlayer->getColor()) {
            cout << "Cannot capture your own piece." << endl;
            return false;
        }

        // 3. Use the piece's own logic (Strategy Pattern) to validate the move
        if (!sourcePiece->canMove(board, *startBox, *endBox)) {
            cout << "Invalid move for this piece." << endl;
            return false;
        }

        // 4. Make the move
        Piece* destPiece = endBox->getPiece();
        endBox->setPiece(sourcePiece);
        startBox->setPiece(nullptr);

        // Delete captured piece
        if (destPiece != nullptr) {
            delete destPiece;
        }

        // 5. Change turn
        currentPlayer = (currentPlayer == &player1) ? &player2 : &player1;
        
        cout << "Move successful." << endl;
        return true;
    }
};

int main() {
    Game& chessGame = Game::getInstance();
    Board& board = chessGame.getBoard();

    // Simple game loop simulation
    cout << "Game started. White's turn." << endl;
    
    // Example: White moves pawn from (1, 4) to (2, 4)
    chessGame.makeMove(1, 4, 2, 4); 

    cout << "\nBlack's turn." << endl;

    // Example: Black moves pawn from (6, 4) to (5, 4)
    chessGame.makeMove(6, 4, 5, 4);

    cout << "\nWhite's turn." << endl;

    // Example: White tries an invalid move for knight (0, 1) to (1, 3)
    chessGame.makeMove(0, 1, 1, 3);
    
    // Example: White makes a valid move for knight (0, 1) to (2, 2)
    chessGame.makeMove(0, 1, 2, 2);

    return 0;
}
