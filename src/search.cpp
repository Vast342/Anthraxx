#include "search.h"
#include "global_includes.h"
#include "lookups.h"

bool timesUp = false;

int winScore = 10000000;
int lossScore = -10000000;

int nodes = 0;

void Engine::scoreMoves(const Board &board, const std::array<Move, 194> &moves, std::array<int, 194> &moveScores, const int totalMoves) {
    uint64_t opponents = board.getBitboard(1 - board.getColorToMove());
    Transposition *entry = tt->getEntry(board.getZobristHash());
    for(int i = 0; i < totalMoves; i++) {
        Move move = moves[i];
        if(move == entry->bestMove) {
            moveScores[i] = 100000000;
        } else {
            uint64_t neighbors = (opponents & neighboringTiles[move.getEndSquare()]);
            moveScores[i] = __builtin_popcountll(neighbors);
            moveScores[i] += (move.getFlag() == Single) * 10;
        }
    }
}

int Engine::negamax(Board &board, int alpha, int beta, int depth, int ply) {
    if(depth <= 0) return board.evaluate();
    /*int state = board.getGameState();
    if(state == Win) return winScore - ply;
    if(state == Loss) return lossScore + ply;
    if(state == Draw) return 0;*/
    if(nodes % 1024 == 0 && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count() > hardLimit) {
        timesUp = true;
        return 0;
    }

    // tt cutoffs and a bunch of other stuff would go here

    std::array<Move, 194> moves;
    std::array<int, 194> moveScores;
    const int totalMoves = board.getMoves(moves);
    scoreMoves(board, moves, moveScores, totalMoves);

    int bestScore = -1000000;
    Move bestMove;
    
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
            }

            if(score >= beta) {
                bestMove = move;
                if(ply == 0) rootBestMove = move;
                break;
            }
        }
        
    }

    tt->pushEntry(Transposition(bestMove), board.getZobristHash());

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
