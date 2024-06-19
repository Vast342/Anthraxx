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
#pragma once
#include "global_includes.h"
#include "move.h"

struct Transposition {
    Transposition() {
        bestMove = Move();
    };
    Transposition(Move move) {
        bestMove = move;
    };
    Move bestMove;
};

constexpr int defaultSize = 64;

struct TT {
    public:
        TT() {
            resize(defaultSize);
            clearTable();
        }
        TT(int newSize) {
            resize(newSize);
            clearTable();
        }
        Transposition* getEntry(uint64_t hash) {
            return &table[hash & mask];
        }
        void pushEntry(Transposition entry, uint64_t hash) {
            table[hash & mask] = entry;
        }
        void clearTable() {
            std::fill(table.begin(), table.end(), Transposition());
        }
        void resize(int newSize) {
            int newSizeMB = newSize;
            int newSizeB = newSizeMB * 1024 * 1024;
            int entrySizeB = sizeof(Transposition);
            int newSizeEntries = newSizeB / entrySizeB;
            mask = newSizeEntries - 1;
            table.resize(newSizeEntries, Transposition());
            clearTable();
        }
        uint64_t mask;
    private:
        std::vector<Transposition> table;
};