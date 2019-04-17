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

#ifndef MOVE_H
#define MOVE_H

#include <stdint.h>
#include "dungeon.h"
#include "input.h"

int move_npc(dungeon_t *, npc *);
int move_pc(dungeon_t *, point_t *, int);
int use_stairs(dungeon_t *, pc_movement_t);
int move_npc_non_tunnel(dungeon_t *, npc *);
int move_npc_tunnel(dungeon_t *, npc *);
int move_npc_erratic(dungeon_t *, npc *);
int check_for_trample(dungeon_t *, character *, int, int);

#endif
