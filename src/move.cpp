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