#pragma once

#include "global_includes.h"
#include "lookups.h"

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