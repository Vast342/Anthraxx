#include "global_includes.h"
#include "board.h"
#include "move.h"

void Board::makeMove(const Move move) {
    stateHistory.push_back(currentState);
    int startSquare = move.getStartSquare();
    int endSquare = move.getEndSquare();
    int flag = move.getFlag();
    assert(flag < 2);
    assert(startSquare < 49);
    assert(endSquare < 49);
    currentState.hundredPlyCounter++;
    currentState.plyCount++;
    switch(flag) {
        case Single:
            currentState.hundredPlyCounter = 0;
            break;
        case Double:
            removeTile(startSquare);
            break;
    }
    addTile(endSquare);
    flipNeighboringTiles(endSquare);
    currentState.sideToMove = 1 - currentState.sideToMove;
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

void Board::removeTile(const int square) {
    assert(square < 49);
    assert(tileAtIndex(square) == currentState.sideToMove);
    const uint64_t squareAsBitboard = 1ULL << square;
    currentState.bitboards[currentState.sideToMove] ^= squareAsBitboard;
}

void Board::flipTile(const int square) {
    assert(square < 49);
    assert(tileAtIndex(square) != None);
    const uint64_t squareAsBitboard = 1ULL << square;
    currentState.bitboards[currentState.sideToMove] ^= squareAsBitboard;
    currentState.bitboards[1 - currentState.sideToMove] ^= squareAsBitboard;
}

void Board::flipNeighboringTiles(const int square) {
    assert(square < 49);
    uint64_t neighbors = (bitboards[1 - currentState.sideToMove] & neighboringTiles[square]);
    assert(((bitboards[X] & bitboards[O]) & neighbors) == neighbors);
    
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
    return __builtin_popcountll(bitboards[currentState.sideToMove]) - __builtin_popcountll(bitboards[1 - currentState.sideToMove])
};
