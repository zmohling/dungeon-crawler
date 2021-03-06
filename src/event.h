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

#ifndef EVENT_SIMULATOR_H
#define EVENT_SIMULATOR_H

#include <stdint.h>
#include "character.h"
#include "dungeon.h"

class dungeon_t;
class character_t;

class event_t {
    public:
    uint32_t turn;
    character_t *c;
};

int event_simulator_start(dungeon_t *);
int event_simulator_stop(dungeon_t *);

#endif
