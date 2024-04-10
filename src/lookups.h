#pragma once

#include "global_includes.h"

constexpr uint64_t fileMask = 0b1000000100000010000001000000100000010000001;
constexpr uint64_t rankMask = 0b1111111;

constexpr uint64_t getFileMask(const int file) {
    return fileMask << file;
}

constexpr uint64_t getRankMask(const int rank) {
    return rankMask << (7 * rank);
};

constexpr uint64_t expand(const int square) {
    const uint64_t squareAsBitboard = 1ULL << square;
    const uint64_t allButLeftMask = ~getFileMask(0);
    const uint64_t allButRightMask = ~getFileMask(6); 
    const uint64_t allButTopMask = ~getRankMask(6);
    const uint64_t allButBottomMask = ~getRankMask(0); 
    return (((squareAsBitboard << 1) & allButLeftMask)
         | ((squareAsBitboard >> 1) & allButRightMask)
         | ((squareAsBitboard << 7) & allButBottomMask)
         | ((squareAsBitboard >> 7) & allButTopMask)
         | ((squareAsBitboard << 6) & (allButBottomMask & allButRightMask))
         | ((squareAsBitboard >> 6) & (allButTopMask & allButLeftMask))
         | ((squareAsBitboard << 8) & (allButBottomMask & allButLeftMask))
         | ((squareAsBitboard >> 8) & (allButTopMask & allButRightMask))) & 0b1111111111111111111111111111111111111111111111111;
}
constexpr uint64_t expandBitboard(const uint64_t bitboard) {
    const uint64_t allButLeftMask = ~getFileMask(0);
    const uint64_t allButRightMask = ~getFileMask(6); 
    const uint64_t allButTopMask = ~getRankMask(6);
    const uint64_t allButBottomMask = ~getRankMask(0); 
    return (((bitboard << 1) & allButLeftMask)
         | ((bitboard >> 1) & allButRightMask)
         | ((bitboard << 7) & allButBottomMask)
         | ((bitboard >> 7) & allButTopMask)
         | ((bitboard << 6) & (allButBottomMask & allButRightMask))
         | ((bitboard >> 6) & (allButTopMask & allButLeftMask))
         | ((bitboard << 8) & (allButBottomMask & allButLeftMask))
         | ((bitboard >> 8) & (allButTopMask & allButRightMask))) & 0b1111111111111111111111111111111111111111111111111;
}

constexpr std::array<uint64_t, 49> generateExpanded() {
    std::array<uint64_t, 49> neighborMasks;
    for(int i = 0; i < 49; i++) {
       neighborMasks[i] = expand(i); 
    }
    return neighborMasks;
}

constexpr uint64_t nextDoor(const int square) {
    const uint64_t squareAsBitboard = 1ULL << square;
    const uint64_t allButLeftMask = ~getFileMask(0) & ~getFileMask(1);
    const uint64_t allButRightMask = ~getFileMask(6) & ~getFileMask(5); 
    const uint64_t allButTopMask = ~getRankMask(6) & ~getRankMask(5);
    const uint64_t allButBottomMask = ~getRankMask(0) & ~getRankMask(1); 
    const uint64_t allButLeftMaskSingle = ~getFileMask(0);
    const uint64_t allButRightMaskSingle = ~getFileMask(6); 
    const uint64_t allButTopMaskSingle = ~getRankMask(6);
    const uint64_t allButBottomMaskSingle = ~getRankMask(0); 
    return (((squareAsBitboard << 5) & (allButBottomMaskSingle & allButRightMask))
         | ((squareAsBitboard << 12) & (allButBottomMask & allButRightMask))
         | ((squareAsBitboard << 13) & (allButBottomMask & allButRightMaskSingle))
         | ((squareAsBitboard << 14) & allButBottomMask)
         | ((squareAsBitboard << 15) & (allButBottomMask & allButLeftMaskSingle))
         | ((squareAsBitboard << 16) & (allButBottomMask & allButLeftMask))
         | ((squareAsBitboard << 9) & (allButBottomMaskSingle & allButLeftMask))
         | ((squareAsBitboard << 2) & allButLeftMask)
         | ((squareAsBitboard >> 5) & (allButTopMaskSingle & allButLeftMask))
         | ((squareAsBitboard >> 12) & (allButTopMask & allButLeftMask))
         | ((squareAsBitboard >> 13) & (allButTopMask & allButLeftMaskSingle))
         | ((squareAsBitboard >> 14) & allButTopMask)
         | ((squareAsBitboard >> 15) & (allButTopMask & allButRightMaskSingle))
         | ((squareAsBitboard >> 16) & (allButTopMask & allButRightMask))
         | ((squareAsBitboard >> 9) & (allButTopMaskSingle & allButRightMask))
         | ((squareAsBitboard >> 2) & allButRightMask)) & 0b1111111111111111111111111111111111111111111111111;
}

constexpr std::array<uint64_t, 49> generateNextDoors() {
    std::array<uint64_t, 49> neighborMasks;
    for(int i = 0; i < 49; i++) {
       neighborMasks[i] = nextDoor(i); 
    }
    return neighborMasks;
}

constexpr std::array<uint64_t, 49> neighboringTiles = generateExpanded();
constexpr std::array<uint64_t, 49> nextDoorTiles = generateNextDoors();
