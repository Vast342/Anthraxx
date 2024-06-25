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
#include "search.h"
#include "global_includes.h"
#include "lookups.h"
#include "movepicker.h" 

bool timesUp = false;

constexpr int winScore = 10000000;
constexpr int lossScore = -10000000;

int nodes = 0;

int Engine::negamax(Board &board, int alpha, int beta, int depth, int ply) {
    if(depth <= 0) return board.getEval();
    // game end state checks
    int state = board.getGameState();
    if(state == Win) return winScore - ply;
    if(state == Loss) return lossScore + ply;
    if(state == Draw) return 0;
    // time check
    if(nodes % 1024 == 0 && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count() > hardLimit) {
        timesUp = true;
        return 0;
    }

    // probe TT
    uint64_t hash = board.getZobristHash();
    Transposition *entry = tt->getEntry(board.getZobristHash());

    // TT Cutoffs, don't do a search again if you've already done it equal or better
    if(ply > 0 && entry->zobristKey == hash && entry->depth >= depth && (
            entry->flag == Exact // exact score
                || (entry->flag == BetaCutoff && entry->score >= beta) // lower bound, fail high
                || (entry->flag == FailLow && entry->score <= alpha) // upper bound, fail low
        )) {
        return entry->score; 
    }

    MovePicker picker;

    // values for saving to TT later
    int bestScore = -1000000;
    Move bestMove;
    int flag = FailLow;
    
    // move loop
    while(picker.hasNext()) {
        Move move = picker.next(board, entry->bestMove);

        // make the move and call the next node        
        board.makeMove(move);
        nodes++;
        const int score = -negamax(board, -beta, -alpha, depth - 1, ply + 1);
        board.undoMove();

        // backup time check
        if(timesUp) return 0;

        if(score > bestScore) {
            bestScore = score;

            if(score > alpha) {
                alpha = score;
                bestMove = move;
                if(ply == 0) rootBestMove = move;
                flag = Exact;
            }

            if(score >= beta) {
                bestMove = move;
                if(ply == 0) rootBestMove = move;
                flag = BetaCutoff;
                break;
            }
        }
        
    }

    // push to TT w/ check to make sure you don't overwrite a move with a null move.
    if(bestMove == Move() && entry->bestMove != Move()) bestMove = entry->bestMove;
    tt->pushEntry(Transposition(hash, bestMove, flag, bestScore, depth), hash);

    return bestScore;
}

// ouputs info for the user to see
void Engine::outputInfo(int score, int depth, int elapsedTime) {
    std::string scoreString = " score cp ";
    scoreString += std::to_string(score);
    std::cout << "info depth " << std::to_string(depth) << " nodes " << std::to_string(nodes) << " time " << std::to_string(elapsedTime) << " nps " << std::to_string(int(double(nodes) / (elapsedTime == 0 ? 1 : elapsedTime) * 1000)) << scoreString << " pv " << rootBestMove.toLongAlgebraic() << std::endl;
}

// searches to higher depths until it's end criteria is met (soon to have aspiration windows)
void Engine::iterativeDeepen(Board board, const int softTimeLimit, const int depth, bool info) {
    for(int i = 1; i <= depth; i++) {
        const Move previousBest = rootBestMove;

        const int score = negamax(board, lossScore, winScore, i, 0);
        
        if(timesUp) {
            rootBestMove = previousBest;
            break;
        }
        const auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count();
        if(info) outputInfo(score, i, elapsedTime);
        if(elapsedTime > softTimeLimit) break;
    }
}

// get a move from the engine, triggers a search
// has parameters for different kinds of searches
Move Engine::think(Board board, const int softTimeLimit, const int hardTimeLimit, const int depth, bool info) {
    hardLimit = hardTimeLimit;
    nodes = 0;
    timesUp = false;

    begin = std::chrono::steady_clock::now();

    iterativeDeepen(board, softTimeLimit, depth, info);
    
    if(info) std::cout << "bestmove " << rootBestMove.toLongAlgebraic() << std::endl;
    return rootBestMove;
}

// the search used for bench, no time limit, just depth and you return the node count.
int Engine::benchSearch(Board board, const int depth) {
    hardLimit = bigNumber;
    nodes = 0;
    timesUp = false;

    begin = std::chrono::steady_clock::now();

    iterativeDeepen(board, bigNumber, depth, false);
    
    return nodes;
}
