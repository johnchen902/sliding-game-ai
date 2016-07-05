/**
 * This file is part of Sliding Game AI
 * Copyright (C) 2016 Pochang Chen
 *
 * Sliding Game AI is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sliding Game AI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DATA_H
#define DATA_H
#include <cstdint>
#include <vector>

using cell_t = std::uint8_t;
using score_t = int64_t;
enum class direction_t { up, down, left, right };

inline direction_t operator - (direction_t d) {
    return static_cast<direction_t>(static_cast<unsigned>(d) ^ 1);
}

template<unsigned width_, unsigned height_>
class Basic_board {
    cell_t data[height_][width_];
public:
    static constexpr unsigned width() { return width_; }
    static constexpr unsigned height() { return height_; }
    cell_t (&operator [] (unsigned i)) [width_] {
        return data[i];
    }
    const cell_t (&operator [] (unsigned i) const) [width_] {
        return data[i];
    }
};

using Standard_board = Basic_board<6, 5>;

template<typename Board>
struct Result {
    score_t score;
    unsigned start_r, start_c;
    std::vector<direction_t> directions;
    Board board;
};

#endif
