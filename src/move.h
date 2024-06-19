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
