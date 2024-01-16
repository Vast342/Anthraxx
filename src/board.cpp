#include "global_includes.h"
#include "board.h"
#include "move.h"

void Board::makeMove(Move move) {
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
