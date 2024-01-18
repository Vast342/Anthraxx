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
    uint64_t stmPieces = currentState.bitboards[currentState.sideToMove];
    const uint64_t emptyBitboard = ~(currentState.bitboards[X] | currentState.bitboards[O]);
    int totalMoves = 0;
    while(stmPieces != 0) {
        const int index = popLSB(stmPieces);
        uint64_t neighbors = neighboringTiles[index] & emptyBitboard;
        uint64_t twoAways = nextDoorTiles[index] & emptyBitboard;
        while(neighbors != 0) {
            const int moveEndSquare = popLSB(neighbors);
            moves[totalMoves] = Move(index, moveEndSquare, Single);
            totalMoves++;   
        }
        while(twoAways != 0) {
            const int moveEndSquare = popLSB(twoAways);
            moves[totalMoves] = Move(index, moveEndSquare, Double);
            totalMoves++;   
        }
    }
    if(totalMoves == 0) {
        moves[totalMoves] = Move(0,0,Passing);
        totalMoves++;
    }
    return totalMoves;
}

// fens have x and o for pieces, and the starting position is x5o/7/7/7/7/7/o5x x 0 1
Board::Board(std::string fen) {
    stateHistory.clear();
    stateHistory.reserve(256);
    for(int i = 0; i < 2; i++) {
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
                default:
                    i += (c - '0');
                    break;
            }
        }
    }
    // convert color to move into 0 or 1, segment 2
    currentState.sideToMove = (segments[1] == "o" ? 1 : 0);
    // 50 move counter, segment 3
    currentState.hundredPlyCounter = 0;
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
    
    while(neighbors != 0) {
        int index = popLSB(neighbors);
        flipTile(index);
    }
}

int Board::tileAtIndex(const int square) const {
    assert(square < 49);
    const uint64_t squareAsBitboard = 1ULL << square;
    for(int i = 0; i < 2; i++) {
        if((currentState.bitboards[i] & squareAsBitboard) != 0) {
            return i;
        }
    }
    return 2;
}

int Board::evaluate() {
    return __builtin_popcountll(currentState.bitboards[currentState.sideToMove]) - __builtin_popcountll(currentState.bitboards[1 - currentState.sideToMove]);
};

int Board::getColorToMove() const {
    return currentState.sideToMove;
}
