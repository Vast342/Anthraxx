#pragma once

#include "global_includes.h"
#include "lookups.h"

inline void runMaskTests() {
    assert(getFileMask(0) == fileMask);
    assert(getFileMask(2) == fileMask << 2);
    assert(getRankMask(0) == rankMask);
    assert(getRankMask(2) == rankMask << 14);
    assert(neighboringTiles[0] == 0b0000000000000000000000000000000000000000110000010);
    assert(neighboringTiles[12] == 0b0000000000000000000000000000111000010100001110000);
    assert(neighboringTiles[30] == 0b0000000000111000010100001110000000000000000000000);
    assert(neighboringTiles[48] == 0b0100000110000000000000000000000000000000000000000);
    std::cout << "Passed" << std::endl;
}