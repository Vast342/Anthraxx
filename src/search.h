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
#pragma once

#include "global_includes.h"
#include "move.h"
#include "board.h"
#include "tt.h"

extern bool timesUp;

struct Engine {
    public: 
        Engine(TT *ttPointer) {
            tt = ttPointer;
        }
        Move think(Board board, const int softTimeLimit, const int hardTimeLimit, const int depth, bool info);
        int benchSearch(Board board, const int depth);
    private:
        int hardLimit;
        Move rootBestMove;
        TT* tt;
        std::chrono::steady_clock::time_point begin;
        void iterativeDeepen(Board board, const int softTimeLimit, const int depth, bool info);
        void scoreMoves(const Board &board, const std::array<Move, 194> &moves, std::array<int, 194> &moveScores, const int totalMoves, const Move ttMove);
        int negamax(Board &board, int alpha, int beta, int depth, int ply);
        void outputInfo(int score, int depth, int elapsedTime);
};