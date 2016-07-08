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

#ifndef IDASTAR_H
#define IDASTAR_H
#include "data.h"
#include <vector>
#include <limits>
#include <atomic>
#include <csignal>

namespace idastar_detail {

std::atomic<bool> stop;

// returns maxmimum score that is rejected
template<typename Callback, typename Board>
score_t dfs(Result<Board> &res, score_t limit,
        unsigned r, unsigned c, Callback callback);

template<typename Callback, typename Board>
inline void dfs1(Result<Board> &res, score_t limit,
        unsigned r1, unsigned c1, Callback callback,
        unsigned r, unsigned c, score_t& reject, direction_t dir) {
    using std::swap;

    if(!res.directions.empty() && res.directions.back() == -dir)
        return;

    res.directions.push_back(dir);
    swap(res.board[r][c], res.board[r1][c1]);

    score_t score = callback(res);
    score_t reject0 = (score >= limit) ? 
            dfs(res, limit, r1, c1, callback) : score;
    if(reject < reject0)
        reject = reject0;

    swap(res.board[r][c], res.board[r1][c1]);
    res.directions.pop_back();
}

template<typename Callback, typename Board>
score_t dfs(Result<Board> &res, score_t limit,
        unsigned r, unsigned c, Callback callback) {
    score_t reject = std::numeric_limits<score_t>::min();

    if(stop)
        return reject;

    if(r > 0)
        dfs1(res, limit, r - 1, c, callback,
                r, c, reject, direction_t::up);
    if(r + 1 < res.board.height())
        dfs1(res, limit, r + 1, c, callback,
                r, c, reject, direction_t::down);
    if(c > 0)
        dfs1(res, limit, r, c - 1, callback,
                r, c, reject, direction_t::left);
    if(c + 1 < res.board.width())
        dfs1(res, limit, r, c + 1, callback,
                r, c, reject, direction_t::right);
    return reject;
}

template<typename Callback, typename Board>
score_t search(Result<Board> &res, score_t limit, Callback callback) {
    score_t reject = std::numeric_limits<score_t>::min();
    for(unsigned r = 0; r < res.board.height(); r++)
        for(unsigned c = 0; c < res.board.width(); c++) {
            res.start_r = r;
            res.start_c = c;
            score_t reject0 = dfs(res, limit, r, c, callback);
            if(reject < reject0)
                reject = reject0;
        }
    return reject;
}

void signal_handler(int) {
    stop = true;
}

template<typename Callback, typename Board>
void idastar(Callback callback, const Board& board) {
    Result<Board> result = {0, 0, 0, {}, board};
    stop = false;
    auto old = std::signal(SIGINT, signal_handler);
    score_t limit = callback(result);
    while(!stop) {
        limit = idastar_detail::search(result, limit, callback);
    }
    std::signal(SIGINT, old);
}

}

template<typename Callback, typename Board>
void idastar(Callback callback, const Board& board) {
    idastar_detail::idastar(callback, board);
}

#endif

