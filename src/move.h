#pragma once
#include "global_includes.h"

enum MoveTypes {
    Single, Double, Passing
};

struct Move {
    public:
        int getStartSquare() const;
        int getEndSquare() const;
        int getFlag() const;
        Move(int start, int end, int type);
        Move(std::string longAlgebraic);
        std::string toLongAlgebraic();
    private:
        uint16_t value;
};
