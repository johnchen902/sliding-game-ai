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

#include <cstdio>
#include <cstdlib>
#include "combo.h"
#include "idastar.h"

template<typename Board>
void solve(const Board &board) {
    Result<Board> result { 0, 0, 0, {}, board };
    auto callback = [&result](const Result<Board> &res) {
        unsigned combo = count_basic_combo(res.board);
        score_t score = static_cast<score_t>(combo);
        if(score > result.score) {
            result = res;
            result.score = score;

            printf("%u %u ", result.start_r, result.start_c);
            for(direction_t d : result.directions)
                switch(d) {
                    case direction_t::up:
                        putchar('^');
                        break;
                    case direction_t::down:
                        putchar('v');
                        break;
                    case direction_t::left:
                        putchar('<');
                        break;
                    case direction_t::right:
                        putchar('>');
                        break;
                    default:
                        __builtin_unreachable();
                }
            putchar('\n');

            if(result.score == 8)
                std::exit(0);
        }
        score_t cost = static_cast<score_t>(res.directions.size());
        return score * 3 - cost;
    };
    idastar(callback, board);
}

int main() {
    constexpr cell_t data[5][6] = {
        1, 2, 2, 3, 2, 4,
        1, 5, 3, 1, 6, 4,
        4, 4, 2, 3, 3, 6,
        2, 5, 5, 2, 5, 4,
        5, 5, 4, 3, 1, 2
    };
    Standard_board board;
    for(unsigned i = 0; i < 5; i++)
        for(unsigned j = 0; j < 6; j++)
            board[i][j] = data[i][j];
    solve(board);
}

