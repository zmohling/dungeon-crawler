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

#ifndef INPUT_H
#define INPUT_H

typedef enum __attribute__((__packed__)) pc_movement_type {
    pc_rest,
    pc_up_stairs,
    pc_down_stairs,
    pc_up,
    pc_up_left,
    pc_up_right,
    pc_left,
    pc_right,
    pc_down,
    pc_down_left,
    pc_down_right,
} pc_movement_t;

class dungeon_t;

void handle_key(dungeon_t *, int);

#endif
