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

#ifndef CHARACTER_H
#define CHARACTER_H

#include <stdbool.h>
#include <stdint.h>

#include "dungeon.h"
#include "geometry.h"

#include <string>

#define NPC_SMART         0x00000001
#define NPC_TELEPATH      0x00000002
#define NPC_TUNNEL        0x00000004
#define NPC_ERRATIC       0x00000008
#define NPC_PASS_WALL     0x00000010
#define NPC_DESTROY_OBJ   0x00000020
#define NPC_PICKUP_OBJ    0x00000040
#define NPC_UNIQ          0x00000080
#define NPC_BOSS          0x00000100

class dungeon_t;
class npc_t {
 public:
     int8_t characteristics;
     bool has_seen_pc;
     point_t pc_position;
};

class character_t {
   public:
    int8_t sequence_num;
    bool is_alive;

    char symbol;
    int8_t speed;

    bool is_pc;
    npc_t *npc;

    point_t position;
};

character_t new_character(dungeon_t *);
bool npc_exists(dungeon_t *);

#endif
