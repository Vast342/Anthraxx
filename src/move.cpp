#include "move.h"

Move::Move(int start, int end, int type) {
    value = start + (end << 6) + (type << 12);
}

int Move::getStartSquare() const {
    return value & 0b111111;
}
int Move::getEndSquare() const {
    return (value >> 6) & 0b111111;
}
int Move::getFlag() const {
    return (value >> 12);
}

Move::Move() {
    value = 0;
}

Move::Move(std::string longAlgebraic) {
    int startSquare = 0;
    int endSquare = 0;
    int flag = Passing;
    // detect the number of letters
    if(longAlgebraic.length() == 2) {
        flag = Single;
        endSquare += longAlgebraic[0] - 'a';
        endSquare += (longAlgebraic[1] - '1') * 7;
    } else if(longAlgebraic.length() == 4) {
        flag = Double;
        startSquare += longAlgebraic[0] - 'a';
        startSquare += (longAlgebraic[1] - '1') * 7;
        endSquare += longAlgebraic[2] - 'a';
        endSquare += (longAlgebraic[3] - '1') * 7;
    }
    value = startSquare + (endSquare << 6) + (flag << 12);
}

constexpr std::array<std::string_view, 49> squareNames = {
    "a1","b1","c1","d1","e1","f1","g1",
    "a2","b2","c2","d2","e2","f2","g2",
    "a3","b3","c3","d3","e3","f3","g3",
    "a4","b4","c4","d4","e4","f4","g4",
    "a5","b5","c5","d5","e5","f5","g5",
    "a6","b6","c6","d6","e6","f6","g6",
    "a7","b7","c7","d7","e7","f7","g7"
};

std::string Move::toLongAlgebraic() {
    std::string longAlgebraic = "";
    if(getFlag() == Double) {
        longAlgebraic += squareNames[getStartSquare()];
        longAlgebraic += squareNames[getEndSquare()];
    } else if(getFlag() == Single) {
        longAlgebraic += squareNames[getEndSquare()];
    } else {
        longAlgebraic = "0000";
    }
    return longAlgebraic;
}
