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
    switch(flag) {
        case Single:
            currentState.hundredPlyCounter = 0;
            addTile(endSquare);
            flipNeighboringTiles(endSquare);
            break;
        case Double:
            removeTile(startSquare);
            addTile(endSquare);
            break;
    }
}

void Board::undoMove() {
    currentState = stateHistory.back();
    stateHistory.pop_back();
}