#pragma once

#include "global_includes.h"

constexpr uint64_t fileMask = 0b1000000100000010000001000000100000010000001;
constexpr uint64_t fileMask = 0b1111111;

constexpr uint64_t getFileMask(const int file) {
    return fileMask << file;
}

constexpr uint64_t getRankMask(const int rank) {
    return fileMask << (7 * rank);
};

constexpr uint64_t expand(const int square) {
    const uint64_t squareAsBitboard = 1ULL << square;
    const uint64_t allButLeftMask = ~getFileMask(0);
    const uint64_t allButRightMask = ~getFileMask(6);
    const uint64_t allButTopMask = ~getRankMask(6);
    const uint64_t allButBottomMask = ~getRankMask(0); 
    return (squareAsBitboard << 1) & allButRightMask
         | (squareAsBitboard >> 1) & allButLeftMask
         | (squareAsBitboard << 7) & allButBottomMask
         | (squareAsBitboard >> 7) & allButTopMask
         | (squareAsBitboard << 6) & (allButBottomMask & allButRightMask)
         | (squareAsBitboard >> 6) & (allButTopMask & allButLeftMask)
         | (squareAsBitboard << 8) & (allButBottomMask & allButLeftMask)
         | (squareAsBitboard >> 8) & (allButTopMask & allButRightMask);
}

constexpr std::array<uint64_t, 49> generateExpanded() {
    std::array<uint64_t, 49> neighborMasks;
    for(int i = 0; i < 49; i++) {
       neighborMasks[i] = expand(i); 
    }
    return neighborMasks;
}

constexpr std::array<uint64_t, 49> neighboringTiles = generateExpanded();
