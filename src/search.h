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
        Move think(Board board, int softTimeLimit, int hardTimeLimit, bool info);
    private:
        int hardLimit;
        Move rootBestMove;
        TT* tt;
        std::chrono::steady_clock::time_point begin;
        void scoreMoves(const Board &board, const std::array<Move, 194> &moves, std::array<int, 194> &moveScores, const int totalMoves);
        int negamax(Board &board, int alpha, int beta, int depth, int ply);
        void outputInfo(int score, int depth, int elapsedTime);
};