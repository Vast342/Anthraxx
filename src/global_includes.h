#pragma once
// includes borrowed from Clarity my chess engine
#include <iostream>
#include <cstdint>
#include <cassert>
#include <string_view>
#include <array>
#include <bit>
#include <ranges>
#include <algorithm>
#include <vector>
#include <string>
#include <sstream>
#include <random>
#include <chrono>
#include <fstream>
#include <memory>

// finds the least significant bit in the uint64_t, gets rid of it, and returns its index
inline int popLSB(uint64_t &bitboard) {
    int lsb = std::countr_zero(bitboard);
    bitboard &= bitboard - 1;
    return lsb;
}

// splits a string into segments based on the seperator
inline std::vector<std::string> split(const std::string string, const char seperator) {
    std::stringstream stream(string);
    std::string segment;
    std::vector<std::string> list;

    // every time that it can get a segment
    while(std::getline(stream, segment, seperator)) {
        // add it to the vector
        list.push_back(segment);
    }
    
    return list;
}
