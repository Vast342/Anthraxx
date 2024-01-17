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
        endSquare += (longAlgebraic[1] - '1') * 8;
    } else if(longAlgebraic.length() == 4) {
        flag = Double;
        startSquare += longAlgebraic[0] - 'a';
        startSquare += (longAlgebraic[1] - '1') * 8;
        endSquare += longAlgebraic[2] - 'a';
        endSquare += (longAlgebraic[3] - '1') * 8;
    }
    value = startSquare + (endSquare << 6) + (flag << 12);
}

std::string Move::toLongAlgebraic() {
    
    return "hehe";
}
