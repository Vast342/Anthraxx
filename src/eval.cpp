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

#include "eval.h"

constexpr int materialWeight = 100;
constexpr std::array<int, 2> tempo = {250, 400};

int evaluate(int sideToMove, const std::array<uint64_t, 3> &bitboards) {
    return materialWeight * (__builtin_popcountll(bitboards[sideToMove]) - __builtin_popcountll(bitboards[1 - sideToMove])) + tempo[sideToMove];
}