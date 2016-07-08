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

#include <algorithm>
#include <ncurses.h>
#include <memory>
#include <set>
#include <utility>
#include "combo.h"
#include "idastar.h"

const char *direction_name(direction_t d) {
    switch(d) {
        case direction_t::up:
            return "up";
        case direction_t::down:
            return "down";
        case direction_t::left:
            return "left";
        case direction_t::right:
            return "right";
        default:
            return "unknown";
    }
}

char cell_representation(cell_t cell) {
    switch(cell) {
        case 0 ... 9:
            return cell + '0';
        case 10 ... 35:
            return cell - 10 + 'a';
        case 36 ... 61:
            return cell - 36 + 'A';
        default:
            return '?';
    }
}

template<typename Board>
void wdisplay_board(WINDOW *window, const Board &board) {
    for(unsigned i = 0; i < board.height(); i++) {
        wmove(window, i + 1, 1);
        for(unsigned j = 0; j < board.width(); j++) {
            cell_t cell = board[i][j];
            if(cell >= 1 && cell <= 6) // XXX magic number
                wattron(window, COLOR_PAIR(cell));
            wprintw(window, "%c", cell_representation(cell));
            if(cell >= 1 && cell <= 6) // XXX magic number
                wattroff(window, COLOR_PAIR(cell));
        }
        wprintw(window, "\n");
    }
    box(window, 0, 0);
}

void wdisplay_directions(WINDOW *window,
        unsigned score, unsigned start_r, unsigned start_c,
        const std::vector<direction_t> &directions) {
    werase(window);
    unsigned sz = directions.size();
    mvwprintw(window, 0, 0,
            "%u moves, score %u, row %u col %u (top/left=1)\n",
            sz, score, start_r + 1, start_c + 1);

    int h, w, y = 1, x = 0;
    getmaxyx(window, h, w);
    for(unsigned i = 0; i < sz; i++) {
        direction_t d = directions[i];
        mvwprintw(window, y, x,
                "%2d: %s", i + 1, direction_name(d));
        if(++y == h) {
            y = 1;
            if((x += 10) + 9 >= w)
                break; // too many item to render
        }
    }
}
void wdisplay_graphics(WINDOW *window,
        unsigned start_r, unsigned start_c,
        const std::vector<direction_t> &directions) {
    werase(window);

    std::set<std::pair<unsigned, unsigned>> set;
    bool reliable = true;
    unsigned y = start_r * 2 + 1;
    unsigned x = start_c * 2 + 1;
    mvwaddch(window, y, x, ACS_BLOCK);
    set.insert(std::make_pair(y, x));
    for(direction_t d : directions) {
        switch(d) {
            case direction_t::up:
                mvwaddch(window, y - 1, x, ACS_UARROW);
                y -= 2;
                break;
            case direction_t::down:
                mvwaddch(window, y + 1, x, ACS_DARROW);
                y += 2;
                break;
            case direction_t::left:
                mvwaddch(window, y, x - 1, ACS_LARROW);
                x -= 2;
                break;
            case direction_t::right:
                mvwaddch(window, y, x + 1, ACS_RARROW);
                x += 2;
                break;
        }
        mvwaddch(window, y, x, ACS_BLOCK);
        if(!set.insert(std::make_pair(y, x)).second)
            reliable = false;
    }

    box(window, 0, 0);
    if(!reliable)
        mvwprintw(window, 0, 1, "unreliable");
}

namespace std {
    template<>
    struct default_delete<WINDOW> {
        void operator()(WINDOW *window) const {
            delwin(window);
        }
    };
}

std::unique_ptr<WINDOW> winput_owner;
std::unique_ptr<WINDOW> winres_owner;
std::unique_ptr<WINDOW> windir_owner;
std::unique_ptr<WINDOW> wingra_owner;

template<typename Board>
void solve(const Board &board) {
    Result<Board> result { 0, 0, 0, {}, board };
    auto callback = [&result](const Result<Board> &res) {
        unsigned combo = count_basic_combo(res.board);
        score_t score = static_cast<score_t>(combo);
        if(score > result.score) {
            result = res;
            result.score = score;

            WINDOW *winres = winres_owner.get();
            WINDOW *wingra = wingra_owner.get();
            WINDOW *windir = windir_owner.get();
            wdisplay_board(winres, result.board);
            wdisplay_graphics(wingra, result.start_r,
                    result.start_c, result.directions);
            wdisplay_directions(windir, result.score,
                    result.start_r, result.start_c,
                    result.directions);
            wnoutrefresh(winres);
            wnoutrefresh(wingra);
            wnoutrefresh(windir);
            doupdate();
        }
        score_t cost = static_cast<score_t>(res.directions.size());
        return score * 3 - cost;
    };
    idastar(callback, board);
}

int main() {
    initscr();
    cbreak();
    // keypad(stdscr, true);
    noecho();

    if(!has_colors()) {
        endwin();
        printf("No color support QAQ\n");
        return 1;
    }

    start_color();
    init_pair(1, COLOR_WHITE, COLOR_RED);
    init_pair(2, COLOR_WHITE, COLOR_GREEN);
    init_pair(3, COLOR_WHITE, COLOR_YELLOW);
    init_pair(4, COLOR_WHITE, COLOR_BLUE);
    init_pair(5, COLOR_WHITE, COLOR_MAGENTA);
    init_pair(6, COLOR_WHITE, COLOR_CYAN);

    printw("Sliding Game AI\n");
    printw("'q' to quit. 's' to solve. SIGINT to stop.\n");
    mvprintw(3, 9, "Copyright (C) 2016 Pochang Chen (johnchen902)");
    mvprintw(4, 9, "This software comes with ABSOLUTELY NO WARRANTY.");
    mvprintw(5, 9, "This is free software, and you are welcome to");
    mvprintw(6, 9, "redistribute it under certain conditions.");
    mvprintw(7, 9, "See the GNU General Public License for more details.");
    refresh();
    
    winput_owner = std::unique_ptr<WINDOW>(newwin(7, 8, 2, 0));
    winres_owner = std::unique_ptr<WINDOW>(newwin(7, 8, 2, 8));
    windir_owner = std::unique_ptr<WINDOW>(newwin(0, 0, 2, 16));
    wingra_owner = std::unique_ptr<WINDOW>(newwin(11, 13, 9, 0));

    WINDOW *winput = winput_owner.get();
    keypad(winput, true);

    Standard_board board = {};
    wdisplay_board(winput, board);
    wmove(winput, 1, 1);
    wrefresh(winput);

    unsigned y = 0, x = 0;
    for(bool quitting = false; !quitting; ) {
        switch(int ch = wgetch(winput)) {
            case KEY_UP:
                if(y > 0)
                    y--;
                else
                    y = board.height() - 1;
                break;
            case KEY_DOWN:
                if(y + 1 < board.height())
                    y++;
                else
                    y = 0;
                break;
            case KEY_LEFT:
                if(x > 0)
                    x--;
                else
                    x = board.width() - 1;
                break;
            case KEY_RIGHT:
                if(x + 1 < board.width())
                    x++;
                else
                    x = 0;
                break;
            case KEY_PPAGE:
                y = 0;
                break;
            case KEY_NPAGE:
                y = board.height() - 1;
                break;
            case KEY_HOME:
                x = 0;
                break;
            case KEY_END:
                x = board.width() - 1;
                break;
            case 'q':
                quitting = true;
                break;
            case 's':
                solve(board);
                break;
            case '0' ... '9':
                board[y][x] = ch - '0';
                break;
        }
        wdisplay_board(winput, board);
        wmove(winput, y + 1, x + 1);
        wrefresh(winput);
    }

    endwin();
}

