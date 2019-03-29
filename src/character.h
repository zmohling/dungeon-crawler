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

#define NPC_INTELLIGENT 0x01
#define NPC_TELEPATHIC 0x02
#define NPC_TUNNELING 0x04
#define NPC_ERRATIC 0x08
#define PC 0x80

class dungeon_t;
class npc_t {
   public:
    int8_t characteristics;

    bool has_seen_pc;
    point_t pc_position;
};

class character_t {
   public:
    char symbol;
    bool is_alive;
    int8_t speed;
    int8_t sequence_num;

    bool is_pc;
    npc_t *npc;

    point_t position;
};

character_t new_character(dungeon_t *);
bool npc_exists(dungeon_t *);

#endif
