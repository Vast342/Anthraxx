#pragma once
#include "global_includes.h"
#include "move.h"

enum Bitboards {
    X, O, Occupied, Empty
};

enum Colors {
    Black, White
};

struct BoardState {
    // x, o, occupied, empty
    std::array<uint64_t, 4> bitboards;
    uint8_t sideToMove;
    uint16_t plyCount;
    uint8_t hundredPlyCounter; 
};

struct Board {
    public:
        Board(std::string fen);
        int getMoves(std::array<Move, 194> moves);
        void makeMove(Move move);
        void undoMove();
    private:
        BoardState currentState;
        std::vector<BoardState> stateHistory;
        void addTile(int square);
        void removeTile(int square);
        void flipTile(int square);
        void flipNeighboringTiles(int square);
        int tileAtIndex(int square);
};