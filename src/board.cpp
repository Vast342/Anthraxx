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

#include "global_includes.h"
#include "board.h"
#include "move.h"
#include "lookups.h"

// zobrist hashing values 
std::array<std::array<uint64_t, 2>, 49> zobTable;
uint64_t zobColorToMove;

// makes a move on the board, and updates all values accordingly
void Board::makeMove(const Move move) {
    stateHistory.push_back(currentState);
    const int startSquare = move.getStartSquare();
    const int endSquare = move.getEndSquare();
    const int flag = move.getFlag();
    assert(flag < 3);
    assert(startSquare < 49);
    assert(endSquare < 49);
    currentState.hundredPlyCounter++;
    currentState.plyCount++;
    switch(flag) {
        case Single:
            currentState.hundredPlyCounter = 0;
            addTile(endSquare);
            flipNeighboringTiles(endSquare);
            break;
        case Double:
            removeTile(startSquare);
            addTile(endSquare);
            flipNeighboringTiles(endSquare);
            break;
    }
    sideToMove = 1 - sideToMove;
    currentState.zobristHash ^= zobColorToMove;
}

// gets all of the moves that are availible in the current position
int Board::getMoves(std::array<Move, 194> &moves) const {
    if(currentState.hundredPlyCounter < 100) {
        uint64_t stmPieces = currentState.bitboards[sideToMove];
        const uint64_t emptyBitboard = ~(currentState.bitboards[X] | currentState.bitboards[O]);
        uint64_t singleMoves = expandBitboard(stmPieces) & emptyBitboard;
        singleMoves ^= (currentState.bitboards[Blocked] & singleMoves);
        int totalMoves = 0;
        while(singleMoves != 0) {
            const int index = popLSB(singleMoves);
            moves[totalMoves] = Move(0, index, Single);
            totalMoves++;   
        }
        while(stmPieces != 0) {
            const int index = popLSB(stmPieces);
            uint64_t twoAways = nextDoorTiles[index] & emptyBitboard;
            twoAways ^= (currentState.bitboards[Blocked] & twoAways);
            while(twoAways != 0) {
                const int moveEndSquare = popLSB(twoAways);
                moves[totalMoves] = Move(index, moveEndSquare, Double);
                totalMoves++;   
            }
        }
        if(totalMoves == 0 && __builtin_popcountll(currentState.bitboards[sideToMove]) != 0) {
            moves[totalMoves] = Move(0,0,Passing);
            totalMoves++;
        }
        return totalMoves;
    }
    return 0;
}

// counts the number of moves in the position, only saves time in perft.
int Board::getMoveCount() const {
    if(currentState.hundredPlyCounter < 100) {
        uint64_t stmPieces = currentState.bitboards[sideToMove];
        const uint64_t emptyBitboard = ~(currentState.bitboards[X] | currentState.bitboards[O]);
        uint64_t singleMoves = expandBitboard(stmPieces) & emptyBitboard;
        singleMoves ^= (currentState.bitboards[Blocked] & singleMoves);
        int totalMoves = 0;
        totalMoves += __builtin_popcountll(singleMoves);
        while(stmPieces != 0) {
            const int index = popLSB(stmPieces);
            uint64_t twoAways = nextDoorTiles[index] & emptyBitboard;
            twoAways ^= (currentState.bitboards[Blocked] & twoAways);
            totalMoves += __builtin_popcountll(twoAways);
        }
        if(totalMoves == 0 && __builtin_popcountll(currentState.bitboards[sideToMove]) != 0) {
            totalMoves++;
        }
        return totalMoves;
    }
    return 0;
}

// loads a fen into the board
// fens have x and o for pieces, and the starting position is x5o/7/7/7/7/7/o5x x 0 1
Board::Board(std::string fen) {
    stateHistory.clear();
    stateHistory.reserve(256);
    for(int i = 0; i < 3; i++) {
        currentState.bitboards[i] = 0ULL;
    }
    currentState.zobristHash = 0;
    // main board state, segment 1
    const std::vector<std::string> segments = split(fen, ' ');
    std::vector<std::string> ranks = split(segments[0], '/');
    std::ranges::reverse(ranks);
    int i = 0;
    for(const auto& rank : ranks) {
        for(char c : rank) {
            switch(c) {
                case 'x':
                    initializeTile(i, X);
                    i++;
                    break;
                case 'o':
                    initializeTile(i, O);
                    i++;
                    break;
                case '-':
                    blockTile(i);
                    i++;
                    break;
                default:
                    i += (c - '0');
                    break;
            }
        }
    }
    // convert color to move into 0 or 1, segment 2
    sideToMove = (segments[1] == "o" ? 1 : 0);
    if(sideToMove == X) currentState.zobristHash ^= zobColorToMove;
    // 50 move counter, segment 3
    currentState.hundredPlyCounter = std::stoi(segments[2]);
    // ply count, segment 4
    currentState.plyCount = std::stoi(segments[3]) * 2 - sideToMove;
}

// undoes most recent move
void Board::undoMove() {
    currentState = stateHistory.back();
    stateHistory.pop_back();
    sideToMove = 1 - sideToMove;
}

// add a tile of the current color on the board
void Board::addTile(const int square) {
    assert(square < 49);
    assert(tileAtIndex(square) == None);
    const uint64_t squareAsBitboard = 1ULL << square;
    currentState.bitboards[sideToMove] ^= squareAsBitboard;
    currentState.zobristHash ^= zobTable[square][sideToMove];
}

// adds a tile of any color to the board
void Board::initializeTile(const int square, const int color) {
    assert(square < 49);
    assert(tileAtIndex(square) == None);
    const uint64_t squareAsBitboard = 1ULL << square;
    currentState.bitboards[color] ^= squareAsBitboard;
    currentState.zobristHash ^= zobTable[square][color];
}

// takes a tile of the current color off of the board
void Board::removeTile(const int square) {
    assert(square < 49);
    assert(tileAtIndex(square) == sideToMove);
    const uint64_t squareAsBitboard = 1ULL << square;
    currentState.bitboards[sideToMove] ^= squareAsBitboard;
    currentState.zobristHash ^= zobTable[square][sideToMove];
}

// blocks a tile so it can't be used
void Board::blockTile(const int square) {
    assert(square < 49);
    const uint64_t squareAsBitboard = 1ULL << square;
    currentState.bitboards[Blocked] ^= squareAsBitboard;
}

// inverts the color of a tile
void Board::flipTile(const int square) {
    assert(square < 49);
    assert(tileAtIndex(square) != sideToMove);
    const uint64_t squareAsBitboard = 1ULL << square;
    currentState.bitboards[sideToMove] ^= squareAsBitboard;
    currentState.bitboards[1 - sideToMove] ^= squareAsBitboard;
    currentState.zobristHash ^= zobTable[square][sideToMove];
    currentState.zobristHash ^= zobTable[square][1 - sideToMove];
}

// inverts the neighboring tiles to a square
void Board::flipNeighboringTiles(const int square) {
    assert(square < 49);
    uint64_t neighbors = (currentState.bitboards[1 - sideToMove] & neighboringTiles[square]);
    
    currentState.bitboards[sideToMove] ^= neighbors;
    currentState.bitboards[1 - sideToMove] ^= neighbors;

    // zobrist hash updates, unfortunately quite a slowdown compared to the efficient update above.
    while(neighbors != 0) {
        int index = popLSB(neighbors);
        currentState.zobristHash ^= zobTable[index][sideToMove];
        currentState.zobristHash ^= zobTable[index][1 - sideToMove];
    }
}

// returns the tile at a specific index
int Board::tileAtIndex(const int square) const {
    assert(square < 49);
    const uint64_t squareAsBitboard = 1ULL << square;
    for(int i = 0; i < 3; i++) {
        if((currentState.bitboards[i] & squareAsBitboard) != 0) {
            return i;
        }
    }
    return None;
}

// returns an evaluation of the board, or how good or bad it is for you.
int Board::getEval() const {
    return 100 * (__builtin_popcountll(currentState.bitboards[sideToMove]) - __builtin_popcountll(currentState.bitboards[1 - sideToMove]));
};

// returns the color currently to move
int Board::getColorToMove() const {
    return sideToMove;
}

// displays the board visually
void Board::toString() const {
    for(int rank = 6; rank >= 0; rank--) {
        for(int file = 0; file < 7; file++) {
            const int tile = tileAtIndex(rank * 7 + file);
            char pieceChar = ' ';
            switch(tile) {
                case X:
                    pieceChar = 'x';
                    break;
                case O:
                    pieceChar = 'o';
                    break;
                case Blocked:
                    pieceChar = '-';
                    break;
            }
            std::cout << pieceChar << " ";
        }
        std::cout << '\n';
    }
    std::cout << "Ply count: " << std::to_string(currentState.plyCount) << '\n';
    std::cout << "Color to move: " << (sideToMove == 1 ? "O" : "X") << '\n';
    std::cout << "Evaluation: " << std::to_string(getEval()) << '\n';
}

// converts the board into a fen string
std::string Board::getFen() const {
    // code originally from the c# version of clarity, then c++ version of clarity, and now here!
    std::string fen = "";
    for(int rank = 6; rank >= 0; rank--) {
        int numEmptyFiles = 0;
        for(int file = 0; file < 7; file++) {
            int piece = tileAtIndex(7*rank+file);
            if(piece != None) {
                if(numEmptyFiles != 0) {
                    fen += std::to_string(numEmptyFiles);
                    numEmptyFiles = 0;
                }
                fen += piece == X ? "x" : piece == O ? "o" : "-";
            }
            else {
                numEmptyFiles++;
            }

        }
        if(numEmptyFiles != 0) {
            fen += std::to_string(numEmptyFiles);
        }
        if(rank != 0) {
            fen += '/';
        }
    }

    // color to move
    fen += ' ';
    fen += (sideToMove == 0 ? 'x' : 'o');
    // 50 move counter
    fen += ' ';
    fen += std::to_string(currentState.hundredPlyCounter / 2);
    // ply count
    fen += ' ';
    fen += std::to_string(currentState.plyCount / 2 + sideToMove);
    return fen;
}

// gets a specific bitboard
uint64_t Board::getBitboard(int bitboard) const {
    return currentState.bitboards[bitboard];
}

// initializes the values for zobrist hashing
void initializeZobrist() {
    // random number stuff
    //std::random_device rd;
    std::mt19937_64 gen(0xABBABA5ED);
    std::uniform_int_distribution<uint64_t> dis;
    // color to move
    zobColorToMove = dis(gen);
    // squares
    for(int i = 0; i < 49; i++) {
        for(int j = 0; j < 2; j++) {
            zobTable[i][j] = dis(gen);
        }
    }
}

// returns the current zobrist hash
uint64_t Board::getZobristHash() const {
    return currentState.zobristHash;
}

// recalculates the zobrist hash and checks that it is identical, for debugging
bool Board::zobristCheck() const {
    uint64_t hash = 0;
    for(int i = 0; i < 49; i++) {
        int piece = tileAtIndex(i);
        if(piece < Blocked) hash ^= zobTable[i][piece];
    }
    if(sideToMove == X) hash ^= zobColorToMove;

    return hash == currentState.zobristHash;
}

// returns a value for if the game has ended or is still going
int Board::getGameState() const {
    const int selfOccupied = __builtin_popcountll(currentState.bitboards[sideToMove]);
    const int opponentOccupied = __builtin_popcountll(currentState.bitboards[1 - sideToMove]);
    const int blockerCount = __builtin_popcountll(currentState.bitboards[Blocked]);

    if(selfOccupied + opponentOccupied + blockerCount == 49) {
        if(selfOccupied > opponentOccupied) {
            return Win;
        } else if(selfOccupied < opponentOccupied) {
            return Loss;
        } else if(selfOccupied == opponentOccupied) {
            return Draw;
        }
    } else if(selfOccupied == 0) {
        return Loss;
    } else if(opponentOccupied == 0) {
        return Win;
    } else if(currentState.hundredPlyCounter >= 100) {
        return Draw;
    }
    return StillGoing;
}

bool Board::isLegal(const Move &move) const {
    if (move == Move()) {
        return false;
    }

    const int from = move.getStartSquare();
    const int to = move.getEndSquare();
    const uint64_t fromBitboard = 1ULL << from;
    const uint64_t toBitboard = 1ULL << to;
    const uint64_t filled = currentState.bitboards[X] | currentState.bitboards[O] | currentState.bitboards[Blocked];
    if (toBitboard & filled) {
        return false;
    }
    if (move.getFlag() == Single) {
        return neighboringTiles[to] & currentState.bitboards[sideToMove];
    }
    return nextDoorTiles[to] & currentState.bitboards[sideToMove] & fromBitboard;
}