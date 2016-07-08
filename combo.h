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

#ifndef COMBO_H
#define COMBO_H
#include "data.h"
#include <bitset>

template<unsigned w, typename Bitset>
Bitset neighbor_mask(Bitset m) {
    return m << (w + 2) | m << 1 | m | m >> 1 | m >> (w + 2);
}

template<typename Board>
unsigned count_basic_combo(const Board &board) {
    constexpr unsigned w = Board::width();
    constexpr unsigned h = Board::height();
    constexpr unsigned max_c = w * (h - 2) + h * (w - 2);

    using Mask = std::bitset<(w + 2) * h - 2>;
    cell_t cells[max_c]; // uninitialized
    Mask masks[max_c];
    unsigned combos = 0;

    for(unsigned i = 0; i < h; i++)
        for(unsigned j = 0; j + 2 < w; j++)
            if(board[i][j] == board[i][j + 1] &&
                    board[i][j + 1] == board[i][j + 2]) {
                cells[combos] = board[i][j];
                masks[combos].reset();
                masks[combos][i * (w + 2) + j] = true;
                masks[combos][i * (w + 2) + j + 1] = true;
                masks[combos][i * (w + 2) + j + 2] = true;
                combos++;
            }
    for(unsigned j = 0; j < w; j++)
        for(unsigned i = 0; i + 2 < h; i++)
            if(board[i][j] == board[i + 1][j] &&
                    board[i + 1][j] == board[i + 2][j]) {
                cells[combos] = board[i][j];
                masks[combos].reset();
                masks[combos][i * (w + 2) + j] = true;
                masks[combos][(i + 1) * (w + 2) + j] = true;
                masks[combos][(i + 2) * (w + 2) + j] = true;
                combos++;
            }

    unsigned count = combos;
    for(unsigned i = 0; i < combos; i++) {
        for(unsigned j = 0; j < i; j++) {
            if(cells[i] == cells[j] &&
                    (neighbor_mask<w>(masks[i]) & masks[j]).any()) {
                masks[i] |= masks[j];
                masks[j].reset();
                count--;
            }
        }
    }
    return count;
}

#endif
