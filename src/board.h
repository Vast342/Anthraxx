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

enum Bitboards {
    X, O, Blocked
};

enum Colors {
    Black, White, Block, None
};

enum GameStates {
    Win, Loss, Draw, StillGoing
};

struct BoardState {
    // x, o
    std::array<uint64_t, 3> bitboards;
    uint64_t zobristHash;
    uint16_t plyCount;
    uint8_t hundredPlyCounter; 
};

struct Board {
    public:
        Board(const std::string fen);
        int getMoves(std::array<Move, 194> &moves);
        int getMoveCount();
        void makeMove(const Move move);
        void undoMove();
        int getEval();
        int getColorToMove() const;
        void toString();
        std::string getFen();
        uint64_t getBitboard(int bitboard) const;
        uint64_t getZobristHash() const;
        int getGameState();
        bool zobristCheck();
    private:
        BoardState currentState;
        uint8_t sideToMove;
        std::vector<BoardState> stateHistory;
        void addTile(const int square);
        void initializeTile(const int square, const int color);
        void removeTile(const int square);
        void blockTile(const int tile);
        void flipTile(const int square);
        void flipNeighboringTiles(const int square);
        int tileAtIndex(const int square) const;
};

void initializeZobrist();