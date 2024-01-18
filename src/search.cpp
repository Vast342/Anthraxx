#include "search.h"
#include "global_includes.h"

int hardLimit;
uint64_t nodes;
bool timesUp;
Move rootBestMove;
std::chrono::steady_clock::time_point begin;

int negamax(Board &board, int alpha, int beta, int depth, int ply) {
    if(depth <= 0) return board.evaluate();
    if(nodes % 4096 == 0 && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count() > hardLimit) {
        timesUp = true;
        return 0;
    }

    // tt cutoffs and a bunch of other stuff would go here

    std::array<Move, 194> moves;
    const int totalMoves = board.getMoves(moves);
    // move ordering would go here

    int bestScore = -1000000;
    
    for(int i = 0; i < totalMoves; i++) {
        // sorting would go here
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
                if(ply == 0) rootBestMove = move;
            }

            if(score >= beta) {
                if(ply == 0) rootBestMove = move;
                break;
            }
        }
        
    }

    return bestScore;
}

void outputInfo(int score, int depth, int elapsedTime) {
    std::string scoreString = " score ";
    scoreString += std::to_string(score);
    std::cout << "info depth " << std::to_string(depth) << " nodes " << std::to_string(nodes) << " time " << std::to_string(elapsedTime) << " nps " << std::to_string(int(double(nodes) / (elapsedTime == 0 ? 1 : elapsedTime) * 1000)) << scoreString << " pv " << rootBestMove.toLongAlgebraic() << std::endl;
}

Move think(Board board, const int softTimeLimit, const int hardTimeLimit) {
    hardLimit = hardTimeLimit;
    nodes = 0;
    timesUp = false;

    begin = std::chrono::steady_clock::now();

    for(int i = 1; i < 100; i++) {
        const Move previousBest = rootBestMove;

        const int score = negamax(board, -10000000, 10000000, i, 0);
        
        if(timesUp) {
            return previousBest;
        }
        const auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count();
        outputInfo(score, i, elapsedTime);
        if(elapsedTime > softTimeLimit) break;
    }
    
    return rootBestMove;
}
