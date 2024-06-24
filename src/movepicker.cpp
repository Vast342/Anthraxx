/*
    Anthraxx
    Copyright (C) 2024 Joseph Pasfield

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "movepicker.h"

/*
    Orders the moves like this:
    1: TT Move
    2: Single Moves (10-18)
    3: Double Moves (0-8)
*/
void MovePicker::scoreMoves(const Board &board, const Move ttMove) {
    uint64_t opponents = board.getBitboard(1 - board.getColorToMove());
    for(int i = 0; i < totalMoves; i++) {
        Move move = moves[i];
        if(move == ttMove) {
            // good move from previous search
            moveScores[i] = 100000000;
        } else {
            // single moves add a tile to the board so are in most cases good (though maybe less in endgames where you want control)
            // captures are also better the more they can capture (again, in most cases)
            uint64_t neighbors = (opponents & neighboringTiles[move.getEndSquare()]);
            moveScores[i] = __builtin_popcountll(neighbors);
            moveScores[i] += (move.getFlag() == Single) * 10;
        }
    }
}

Move MovePicker::next(const Board &board, const Move ttMove) {
    if(phase == ReadyToInit) {
        // initializes
        totalMoves = board.getMoves(moves);
        scoreMoves(board, ttMove);
        phase++;
    }
    if(movesGotten == totalMoves) return Move();
    // Incremental Sorting
    for(int j = movesGotten + 1; j < totalMoves; j++) {
        if(moveScores[j] > moveScores[movesGotten]) {
            std::swap(moveScores[j], moveScores[movesGotten]);
            std::swap(moves[j], moves[movesGotten]);
        }
    }
    movesGotten++;
    return moves[movesGotten - 1];
}