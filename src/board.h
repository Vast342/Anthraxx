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
    uint8_t sideToMove;
    uint16_t plyCount;
    uint8_t hundredPlyCounter; 
    uint64_t zobristHash;
};

struct Board {
    public:
        Board(const std::string fen);
        int getMoves(std::array<Move, 194> &moves);
        int getMoveCount();
        void makeMove(const Move move);
        void undoMove();
        int evaluate();
        int getColorToMove() const;
        void toString();
        std::string getFen();
        uint64_t getBitboard(int bitboard) const;
        uint64_t getZobristHash() const;
        int getGameState();
        bool zobristCheck();
    private:
        BoardState currentState;
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