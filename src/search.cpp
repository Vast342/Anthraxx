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

bool timesUp = false;

constexpr int winScore = 10000000;
constexpr int lossScore = -10000000;

int nodes = 0;

/*
    1: TT Move
    2: Single Moves (10-18)
    3: Double Moves (0-8)
*/
void Engine::scoreMoves(const Board &board, const std::array<Move, 194> &moves, std::array<int, 194> &moveScores, const int totalMoves, const Move ttMove) {
    uint64_t opponents = board.getBitboard(1 - board.getColorToMove());
    for(int i = 0; i < totalMoves; i++) {
        Move move = moves[i];
        if(move == ttMove) {
            moveScores[i] = 100000000;
        } else {
            uint64_t neighbors = (opponents & neighboringTiles[move.getEndSquare()]);
            moveScores[i] = __builtin_popcountll(neighbors);
            moveScores[i] += (move.getFlag() == Single) * 10;
        }
    }
}

int Engine::negamax(Board &board, int alpha, int beta, int depth, int ply) {
    if(depth <= 0) return board.getEval();
    int state = board.getGameState();
    if(state == Win) return winScore - ply;
    if(state == Loss) return lossScore + ply;
    if(state == Draw) return 0;
    if(nodes % 1024 == 0 && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count() > hardLimit) {
        timesUp = true;
        return 0;
    }

    uint64_t hash = board.getZobristHash();
    Transposition *entry = tt->getEntry(board.getZobristHash());

    if(ply > 0 && entry->zobristKey == hash && entry->depth >= depth && (
            entry->flag == Exact // exact score
                || (entry->flag == BetaCutoff && entry->score >= beta) // lower bound, fail high
                || (entry->flag == FailLow && entry->score <= alpha) // upper bound, fail low
        )) {
        return entry->score; 
    }

    std::array<Move, 194> moves;
    std::array<int, 194> moveScores;
    const int totalMoves = board.getMoves(moves);
    scoreMoves(board, moves, moveScores, totalMoves, entry->bestMove);

    int bestScore = -1000000;
    Move bestMove;
    int flag = FailLow;
    
    for(int i = 0; i < totalMoves; i++) {
        // Incremental Sorting
        for(int j = i + 1; j < totalMoves; j++) {
            if(moveScores[j] > moveScores[i]) {
                std::swap(moveScores[j], moveScores[i]);
                std::swap(moves[j], moves[i]);
            }
        }

        Move move = moves[i];
        // buncha pruning goes here
        board.makeMove(move);
        nodes++;
        const int score = -negamax(board, -beta, -alpha, depth - 1, ply + 1);
        board.undoMove();

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

    if(bestMove == Move() && entry->bestMove != Move()) bestMove = entry->bestMove;
    tt->pushEntry(Transposition(hash, bestMove, flag, bestScore, depth), hash);

    return bestScore;
}

void Engine::outputInfo(int score, int depth, int elapsedTime) {
    std::string scoreString = " score cp ";
    scoreString += std::to_string(score);
    std::cout << "info depth " << std::to_string(depth) << " nodes " << std::to_string(nodes) << " time " << std::to_string(elapsedTime) << " nps " << std::to_string(int(double(nodes) / (elapsedTime == 0 ? 1 : elapsedTime) * 1000)) << scoreString << " pv " << rootBestMove.toLongAlgebraic() << std::endl;
}

Move Engine::think(Board board, const int softTimeLimit, const int hardTimeLimit, bool info) {
    hardLimit = hardTimeLimit;
    nodes = 0;
    timesUp = false;

    begin = std::chrono::steady_clock::now();

    for(int i = 1; i < 100; i++) {
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
    
    if(info) std::cout << "bestmove " << rootBestMove.toLongAlgebraic() << std::endl;
    return rootBestMove;
}

Move Engine::fixedDepthSearch(Board board, const int depth, const bool info) {
    hardLimit = 1215752192;
    nodes = 0;
    timesUp = false;

    begin = std::chrono::steady_clock::now();

    for(int i = 1; i <= depth; i++) {
        const Move previousBest = rootBestMove;

        const int score = negamax(board, lossScore, winScore, i, 0);
        
        if(timesUp) {
            rootBestMove = previousBest;
            break;
        }
        const auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count();
        if(info) outputInfo(score, i, elapsedTime);
    }
    
    if(info) std::cout << "bestmove " << rootBestMove.toLongAlgebraic() << std::endl;
    return rootBestMove;
}

int Engine::benchSearch(Board board, const int depth) {
    hardLimit = 1215752192;
    nodes = 0;
    timesUp = false;

    begin = std::chrono::steady_clock::now();

    for(int i = 1; i <= depth; i++) {
        negamax(board, lossScore, winScore, i, 0);
    }
    return nodes;
}
