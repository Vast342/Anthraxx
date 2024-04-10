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
        Move();
        std::string toLongAlgebraic();
        [[nodiscard]] constexpr auto operator==(const Move &other) const -> bool = default;
    private:
        uint16_t value;
};
