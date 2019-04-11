/*
 *  Copyright (C) 2019 Zachary Mohling
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "input.h"

#include <ncurses.h>
#include <stdlib.h>

#include "accessory_screens.h"
#include "debug.h"
#include "dungeon_crawler.h"
#include "fov.h"
#include "move.h"

static void get_new_input(int *key_is_valid, int *input) {
    *key_is_valid = 0;
    *input = getch();

    return;
}

void handle_key(dungeon_t *d, int c) {
    int input = c;
    int key_is_valid;

    do {
        key_is_valid = 1;

        /*Clear status line */
        move(0, 0);
        clrtoeol();

        switch (input) {
            case '7':
            case 'y':
            case '8':
            case 'k':
            case '9':
            case 'u':
            case '6':
            case 'l':
            case '3':
            case 'n':
            case '2':
            case 'j':
            case '1':
            case 'b':
            case '4':
            case 'h':
            case '5':
            case '.':
            case ' ':
                if (move_pc(d, NULL, input)) {
                    /* Invalid */
                    mvprintw(0, 0, "%s", "You cannot move here!");
                    get_new_input(&key_is_valid, &input);
                }
                break;
            case '>':
                if (d->map[d->pc->position.y][d->pc->position.x] ==
                    ter_stairs_down) {
                    /* Valid */
                    use_stairs(d, pc_down_stairs);
                } else {
                    /* Invalid */
                    mvprintw(0, 0, "%s", "You cannot go down the stairs here!");
                    get_new_input(&key_is_valid, &input);
                }
                break;
            case '<':
                if (d->map[d->pc->position.y][d->pc->position.x] ==
                    ter_stairs_up) {
                    /* Valid */
                    use_stairs(d, pc_up_stairs);
                } else {
                    /* Invalid */
                    mvprintw(0, 0, "%s", "You cannot go up the stairs here!");
                    get_new_input(&key_is_valid, &input);
                }
                break;
            case 'm':
                monster_list(d);
                render_dungeon(d);
                get_new_input(&key_is_valid, &input);
                break;
            case KEY_UP:
                break;
            case KEY_DOWN:
                break;
            case 'q':
                quit();
                break;
            case 'f':
                FOV_toggle_fog();
                mvprintw(0, 0, "%s", "Fog of War was toggled.");
                get_new_input(&key_is_valid, &input);
                break;
            case 't':
                mvprintw(0, 0, "%s", "Teleport Mode");
                DEBUG_teleport_mode(d);
                break;
            default:
                mvprintw(0, 0, "%s", "Invalid Input");
                get_new_input(&key_is_valid, &input);
                break;
        }

    } while (!key_is_valid);
}
