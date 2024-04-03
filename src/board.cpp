#include "global_includes.h"
#include "board.h"
#include "move.h"
#include "lookups.h"

void Board::makeMove(const Move move) {
    stateHistory.push_back(currentState);
    const int startSquare = move.getStartSquare();
    const int endSquare = move.getEndSquare();
    const int flag = move.getFlag();
    assert(flag < 3);
    assert(startSquare < 49);
    assert(endSquare < 49);
    currentState.hundredPlyCounter++;
    currentState.plyCount++;
    switch(flag) {
        case Single:
            currentState.hundredPlyCounter = 0;
            addTile(endSquare);
            flipNeighboringTiles(endSquare);
            break;
        case Double:
            removeTile(startSquare);
            addTile(endSquare);
            flipNeighboringTiles(endSquare);
            break;
    }
    currentState.sideToMove = 1 - currentState.sideToMove;
}

int Board::getMoves(std::array<Move, 194> &moves) {
    if(currentState.hundredPlyCounter < 100) {
        uint64_t stmPieces = currentState.bitboards[currentState.sideToMove];
        const uint64_t emptyBitboard = ~(currentState.bitboards[X] | currentState.bitboards[O]);
        uint64_t singleMoves = expandBitboard(stmPieces) & emptyBitboard;
        singleMoves ^= (currentState.bitboards[Blocked] & singleMoves);
        int totalMoves = 0;
        while(singleMoves != 0) {
            const int index = popLSB(singleMoves);
            moves[totalMoves] = Move(0, index, Single);
            totalMoves++;   
        }
        while(stmPieces != 0) {
            const int index = popLSB(stmPieces);
            uint64_t twoAways = nextDoorTiles[index] & emptyBitboard;
            twoAways ^= (currentState.bitboards[Blocked] & twoAways);
            while(twoAways != 0) {
                const int moveEndSquare = popLSB(twoAways);
                moves[totalMoves] = Move(index, moveEndSquare, Double);
                totalMoves++;   
            }
        }
        if(totalMoves == 0 && __builtin_popcountll(currentState.bitboards[currentState.sideToMove]) != 0) {
            moves[totalMoves] = Move(0,0,Passing);
            totalMoves++;
        }
        return totalMoves;
    }
    return 0;
}

int Board::getMoveCount() {
    if(currentState.hundredPlyCounter < 100) {
        uint64_t stmPieces = currentState.bitboards[currentState.sideToMove];
        const uint64_t emptyBitboard = ~(currentState.bitboards[X] | currentState.bitboards[O]);
        uint64_t singleMoves = expandBitboard(stmPieces) & emptyBitboard;
        singleMoves ^= (currentState.bitboards[Blocked] & singleMoves);
        int totalMoves = 0;
        totalMoves += __builtin_popcountll(singleMoves);
        while(stmPieces != 0) {
            const int index = popLSB(stmPieces);
            uint64_t twoAways = nextDoorTiles[index] & emptyBitboard;
            twoAways ^= (currentState.bitboards[Blocked] & twoAways);
            totalMoves += __builtin_popcountll(twoAways);
        }
        if(totalMoves == 0 && __builtin_popcountll(currentState.bitboards[currentState.sideToMove]) != 0) {
            totalMoves++;
        }
        return totalMoves;
    }
    return 0;
}

// fens have x and o for pieces, and the starting position is x5o/7/7/7/7/7/o5x x 0 1
Board::Board(std::string fen) {
    stateHistory.clear();
    stateHistory.reserve(256);
    for(int i = 0; i < 3; i++) {
        currentState.bitboards[i] = 0ULL;
    }
    // main board state, segment 1
    const std::vector<std::string> segments = split(fen, ' ');
    std::vector<std::string> ranks = split(segments[0], '/');
    std::ranges::reverse(ranks);
    int i = 0;
    for(const auto& rank : ranks) {
        for(char c : rank) {
            switch(c) {
                case 'x':
                    initializeTile(i, X);
                    i++;
                    break;
                case 'o':
                    initializeTile(i, O);
                    i++;
                    break;
                case '-':
                    blockTile(i);
                    i++;
                    break;
                default:
                    i += (c - '0');
                    break;
            }
        }
    }
    // convert color to move into 0 or 1, segment 2
    currentState.sideToMove = (segments[1] == "o" ? 1 : 0);
    // 50 move counter, segment 3
    currentState.hundredPlyCounter = std::stoi(segments[2]);
    // ply count, segment 4
    currentState.plyCount = std::stoi(segments[3]) * 2 - currentState.sideToMove;
}

void Board::undoMove() {
    currentState = stateHistory.back();
    stateHistory.pop_back();
}

void Board::addTile(const int square) {
    assert(square < 49);
    assert(tileAtIndex(square) == None);
    const uint64_t squareAsBitboard = 1ULL << square;
    currentState.bitboards[currentState.sideToMove] ^= squareAsBitboard;
}

void Board::initializeTile(const int square, const int color) {
    assert(square < 49);
    assert(tileAtIndex(square) == None);
    const uint64_t squareAsBitboard = 1ULL << square;
    currentState.bitboards[color] ^= squareAsBitboard;
}

void Board::removeTile(const int square) {
    assert(square < 49);
    assert(tileAtIndex(square) == currentState.sideToMove);
    const uint64_t squareAsBitboard = 1ULL << square;
    currentState.bitboards[currentState.sideToMove] ^= squareAsBitboard;
}

void Board::blockTile(const int square) {
    assert(square < 49);
    const uint64_t squareAsBitboard = 1ULL << square;
    currentState.bitboards[Blocked] ^= squareAsBitboard;
}

void Board::flipTile(const int square) {
    assert(square < 49);
    assert(tileAtIndex(square) != currentState.sideToMove);
    const uint64_t squareAsBitboard = 1ULL << square;
    currentState.bitboards[currentState.sideToMove] ^= squareAsBitboard;
    currentState.bitboards[1 - currentState.sideToMove] ^= squareAsBitboard;
}

void Board::flipNeighboringTiles(const int square) {
    assert(square < 49);
    uint64_t neighbors = (currentState.bitboards[1 - currentState.sideToMove] & neighboringTiles[square]);
    
    currentState.bitboards[currentState.sideToMove] ^= neighbors;
    currentState.bitboards[1 - currentState.sideToMove] ^= neighbors;
}

int Board::tileAtIndex(const int square) const {
    assert(square < 49);
    const uint64_t squareAsBitboard = 1ULL << square;
    for(int i = 0; i < 3; i++) {
        if((currentState.bitboards[i] & squareAsBitboard) != 0) {
            return i;
        }
    }
    return None;
}

int Board::evaluate() {
    return __builtin_popcountll(currentState.bitboards[currentState.sideToMove]) - __builtin_popcountll(currentState.bitboards[1 - currentState.sideToMove]);
};

int Board::getColorToMove() const {
    return currentState.sideToMove;
}

void Board::toString() {
    for(int rank = 6; rank >= 0; rank--) {
        for(int file = 0; file < 7; file++) {
            const int tile = tileAtIndex(rank * 7 + file);
            char pieceChar = ' ';
            switch(tile) {
                case X:
                    pieceChar = 'x';
                    break;
                case O:
                    pieceChar = 'o';
                    break;
                case Blocked:
                    pieceChar = '-';
                    break;
            }
            std::cout << pieceChar << " ";
        }
        std::cout << '\n';
    }
    std::cout << "Ply count: " << std::to_string(currentState.plyCount) << '\n';
    std::cout << "Color to move: " << (currentState.sideToMove == 1 ? "O" : "X") << '\n';
    std::cout << "Evaluation: " << std::to_string(evaluate()) << '\n';
}

std::string Board::getFen() {
    // code originally from the c# version of clarity, then c++ version of clarity, and now here!
    std::string fen = "";
    for(int rank = 6; rank >= 0; rank--) {
        int numEmptyFiles = 0;
        for(int file = 0; file < 7; file++) {
            int piece = tileAtIndex(7*rank+file);
            if(piece != None) {
                if(numEmptyFiles != 0) {
                    fen += std::to_string(numEmptyFiles);
                    numEmptyFiles = 0;
                }
                fen += piece == X ? "x" : piece == O ? "o" : "-";
            }
            else {
                numEmptyFiles++;
            }

        }
        if(numEmptyFiles != 0) {
            fen += std::to_string(numEmptyFiles);
        }
        if(rank != 0) {
            fen += '/';
        }
    }

    // color to move
    fen += ' ';
    fen += (currentState.sideToMove == 0 ? 'x' : 'o');
    // 50 move counter
    fen += ' ';
    fen += std::to_string(currentState.hundredPlyCounter / 2);
    // ply count
    fen += ' ';
    fen += std::to_string(currentState.plyCount / 2 + currentState.sideToMove);
    return fen;
}