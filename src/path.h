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

#ifndef PATHFINDER_H
#define PATHFINDER_H

#include "dungeon.h"
#include "heap.h"

#define mapxy(x, y) (d->map[y][x])
#define tunnel_cost(x, y)             \
    (d->hardness_map[y][x] < (85 * 1) \
         ? 1                          \
         : (d->hardness_map[y][x] < (85 * 2) + 1 ? 2 : 3))

typedef struct graph_node {
    heap_node_t *hn;
    point_t position;
} graph_node_t;

void non_tunnel_distance_map(dungeon_t *d);
void tunnel_distance_map(dungeon_t *d);

#endif
