#pragma once

#include "global_includes.h"

constexpr uint64_t expand(const int square) {
    const uint64_t squareAsBitboard = 1ULL << square;
    return squareAsBitboard << 1
         & squareAsBitboard >> 1
         & squareAsBitboard << 7
         & squareAsBitboard >> 7
         & squareAsBitboard << 6
         & squareAsBitboard >> 6
         & squareAsBitboard << 8
         & squareAsBitboard >> 8;
}

constexpr std::array<uint64_t, 49> generateExpanded() {
    std::array<uint64_t, 49> neighborMasks;
    for(int i = 0; i < 49; i++) {
       neighborMasks[i] = expand(i); 
    }
    return neighborMasks;
}

constexpr std::array<uint64_t, 49> neighboringTiles = generateExpanded();
