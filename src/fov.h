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

#ifndef FOV_H
#define FOV_H

#define FOV_SIGHT_RADIUS 5

#include "stdbool.h"

class dungeon_t;
class point_t;
class slope_pair_t {
    public:
    double start_slope;
    double end_slope;
};

void FOV_recursive_shadowcast(dungeon_t *, point_t *, slope_pair_t, int, int,
                              int);
void FOV_shadowcast(dungeon_t *, point_t *, int);
void FOV_clear(dungeon_t *);
bool FOV_get_fog();
void FOV_toggle_fog();

#endif
