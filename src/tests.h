#pragma once

#include "global_includes.h"
#include "lookups.h"
#include "move.h"
#include "board.h"

inline void runMaskTests() {
    std::cout << "testing masks: " << std::endl;
    assert(getFileMask(0) == fileMask);
    assert(getFileMask(2) == fileMask << 2);
    assert(getRankMask(0) == rankMask);
    assert(getRankMask(2) == rankMask << 14);
    std::cout << "rank/file functionality check: PASSED" << std::endl;
    assert(neighboringTiles[0] == 0b0000000000000000000000000000000000000000110000010);
    std::cout << "neighboring tile test 1: PASSED" << std::endl;
    assert(neighboringTiles[12] == 0b0000000000000000000000000000111000010100001110000);
    std::cout << "neighboring tile test 2: PASSED" << std::endl;
    assert(neighboringTiles[30] == 0b0000000000111000010100001110000000000000000000000);
    std::cout << "neighboring tile test 3: PASSED" << std::endl;
    assert(neighboringTiles[48] == 0b0100000110000000000000000000000000000000000000000);
    std::cout << "neighboring tile test 4: PASSED" << std::endl;
    std::cout << "neighboring tile tests: PASSED" << std::endl;
    assert(nextDoorTiles[0] == 0b0000000000000000000000000000000011100001000000100);
    std::cout << "2 away tile test 1: PASSED" << std::endl;
    assert(nextDoorTiles[12] == 0b0000000000000000000001111000000100000010000001000);
    std::cout << "2 away tile test 2: PASSED" << std::endl;
    assert(nextDoorTiles[30] == 0b0011111001000100100010010001001111100000000000000);
    std::cout << "2 away tile test 3: PASSED" << std::endl;
    assert(nextDoorTiles[48] == 0b0010000001000011100000000000000000000000000000000);
    std::cout << "2 away tile test 4: PASSED" << std::endl;
    std::cout << "2 away tile tests: PASSED" << std::endl;
    std::cout << "All Tests Passed" << std::endl;
}

inline uint64_t perft(Board &board, const int depth) {
    // YIPPY bulk counting
    if(depth == 1) return board.getMoveCount();
    if(depth <= 0) return 1;
    std::array<Move, 194> moves;
    const int numMoves = board.getMoves(moves);
    uint64_t result = 0;
    for(int i = 0; i < numMoves; i++) {
        board.makeMove(moves[i]);
        result += perft(board, depth-1);
        board.undoMove();
    }
    return result;
}

inline void runPerftTest(Board board, const int depth) {
    clock_t start = clock();
    const uint64_t result = perft(board, depth);
    clock_t end = clock();
    std::cout << "Result: " << std::to_string(result) << '\n';
    std::cout << "Time: " << std::to_string((end-start)/static_cast<double>(1000)) << '\n';
    std::cout << "NPS: " << std::to_string(result / ((end-start)/static_cast<double>(1000))) << '\n';
}

const std::pair<std::string, std::vector<int>> positions[] = {
    {"7/7/7/7/7/7/7 x 0 1", {1, 0, 0, 0, 0}},
    {"7/7/7/7/7/7/7 o 0 1", {1, 0, 0, 0, 0}},
    {"x5o/7/7/7/7/7/o5x x 0 1", {1, 16, 256, 6460, 155888, 4752668}},
    {"x5o/7/7/7/7/7/o5x o 0 1", {1, 16, 256, 6460, 155888, 4752668}},
    {"x5o/7/2-1-2/7/2-1-2/7/o5x x 0 1", {1, 14, 196, 4184, 86528, 2266352}},
    {"x5o/7/2-1-2/7/2-1-2/7/o5x o 0 1", {1, 14, 196, 4184, 86528, 2266352}},
    {"x5o/7/2-1-2/3-3/2-1-2/7/o5x x 0 1", {1, 14, 196, 4100, 83104, 2114588}},
    {"x5o/7/2-1-2/3-3/2-1-2/7/o5x o 0 1", {1, 14, 196, 4100, 83104, 2114588}},
    {"x5o/7/3-3/2-1-2/3-3/7/o5x x 0 1", {1, 16, 256, 5948, 133264, 3639856}},
    {"x5o/7/3-3/2-1-2/3-3/7/o5x o 0 1", {1, 16, 256, 5948, 133264, 3639856}},
    {"7/7/7/7/ooooooo/ooooooo/xxxxxxx x 0 1", {1, 1, 75, 249, 14270, 452980}},
    {"7/7/7/7/ooooooo/ooooooo/xxxxxxx o 0 1", {1, 75, 249, 14270, 452980}},
    {"7/7/7/7/xxxxxxx/xxxxxxx/ooooooo x 0 1", {1, 75, 249, 14270, 452980}},
    {"7/7/7/7/xxxxxxx/xxxxxxx/ooooooo o 0 1", {1, 1, 75, 249, 14270, 452980}},
    {"7/7/7/2x1o2/7/7/7 x 0 1", {1, 23, 419, 7887, 168317, 4266992}},
    {"7/7/7/2x1o2/7/7/7 o 0 1", {1, 23, 419, 7887, 168317, 4266992}},
    {"x5o/7/7/7/7/7/o5x x 100 1", {1, 0, 0, 0, 0}},
    {"x5o/7/7/7/7/7/o5x o 100 1", {1, 0, 0, 0, 0}},
    {"7/7/7/7/-------/-------/x5o x 0 1", {1, 2, 4, 13, 30, 73, 174}},
    {"7/7/7/7/-------/-------/x5o o 0 1", {1, 2, 4, 13, 30, 73, 174}},
};

inline void runPerftSuite() {
    int j = 0;
    for(const auto& [fen, nodes] : positions) {
        Board board(fen);
        for(unsigned int i = 0; i < nodes.size(); ++i) {
            j++;
            int result = perft(board, i);
            if(result == nodes[i]) {
                std::cout << "Passed test number " << j << std::endl;
            } else {
                std::cout << "Failed test number " << j << " with fen " << fen << " and output " << result << ", expected result was " << nodes[i] << std::endl;
            }
        }
    }
}

inline void runSplitPerft(Board board, const int depth) {
    clock_t start = clock();
    std::array<Move, 194> moves;
    const int numMoves = board.getMoves(moves);
    uint64_t result = 0;
    for(int i = 0; i < numMoves; i++) {
        board.makeMove(moves[i]);
        const int currentResult = perft(board, depth - 1);
        result += currentResult;
        std::cout << moves[i].toLongAlgebraic() << ": " << currentResult << std::endl;
        board.undoMove();
    }
    clock_t end = clock();
    std::cout << "Result: " << std::to_string(result) << '\n';
    std::cout << "Time: " << std::to_string((end-start)/static_cast<double>(1000)) << '\n';
    std::cout << "NPS: " << std::to_string(result / ((end-start)/static_cast<double>(1000))) << '\n';
}