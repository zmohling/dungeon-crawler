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

#include "debug.h"

#include <ncurses.h>

#include "dungeon.h"
#include "fov.h"
#include "move.h"

static void DEBUG_print_teleport_pointer(dungeon_t *d, point_t pointer) {
    attron(A_BOLD);
    attron(COLOR_PAIR(2));
    mvprintw(pointer.y + 1, pointer.x, "%c", '*');
    attron(COLOR_PAIR(1));
    attroff(A_BOLD);

    mvprintw(
        0, 0, "%s",
        "TELEPORT MODE: Press 't' to teleport to pointer, 'r' for random.");
}

static void DEBUG_teleport_get_new_input(int *key_is_valid, int *input) {
    *key_is_valid = 0;
    *input = getch();

    return;
}

static point_t DEBUG_teleport_handle_key(dungeon_t *d, int c) {
    int input = c;
    int key_is_valid;

    point_t pointer;
    pointer.x = d->pc->position.x;
    pointer.y = d->pc->position.y;

    do {
        key_is_valid = 1;

        /*Clear status line */
        move(0, 0);
        clrtoeol();

        switch (input) {
            case '7':
            case 'y':
                pointer.x = pointer.x - 1;
                pointer.y = pointer.y - 1;
                break;
            case '8':
            case 'k':
                pointer.x = pointer.x;
                pointer.y = pointer.y - 1;
                break;
            case '9':
            case 'u':
                pointer.x = pointer.x + 1;
                pointer.y = pointer.y - 1;
                break;
            case '6':
            case 'l':
                pointer.x = pointer.x + 1;
                pointer.y = pointer.y;
                break;
            case '3':
            case 'n':
                pointer.x = pointer.x + 1;
                pointer.y = pointer.y + 1;
                break;
            case '2':
            case 'j':
                pointer.x = pointer.x;
                pointer.y = pointer.y + 1;
                break;
            case '1':
            case 'b':
                pointer.x = pointer.x - 1;
                pointer.y = pointer.y + 1;
                break;
            case '4':
            case 'h':
                pointer.x = pointer.x - 1;
                pointer.y = pointer.y;
                break;
            case 't':
                return pointer;
                break;
            case 'r':
                pointer = get_valid_point(d, true);
                return pointer;
                break;
            default:
                mvprintw(0, 0, "%s", "Invalid Input");
                DEBUG_teleport_get_new_input(&key_is_valid, &input);
                break;
        }

        render_dungeon(d);
        DEBUG_print_teleport_pointer(d, pointer);
        DEBUG_teleport_get_new_input(&key_is_valid, &input);
    } while (!key_is_valid);

    /* Shouldn't reach here */
    return pointer;
}

void DEBUG_teleport_mode(dungeon_t *d) {
    if (FOV_get_fog()) {
        FOV_toggle_fog();
    }

    DEBUG_print_teleport_pointer(d, d->pc->position);

    point_t next_pos = DEBUG_teleport_handle_key(d, getch());
    while (move_pc(d, &next_pos, ' ')) {
        /* Invalid */
        mvprintw(0, 0, "%s", "You cannot move here!");
        next_pos = DEBUG_teleport_handle_key(d, getch());
    }

    FOV_toggle_fog();
}
