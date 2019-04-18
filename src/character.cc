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

#include "character.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "dungeon.h"

// Initialize static data members.
pc *pc::s_instance = 0;
pc *pc::instance() {
  if (!s_instance) s_instance = new pc;
  return s_instance;
}

/* Generates and returns a new character. If it's the first character in
 * the dungeon's characters array, will generate a PC character.
 * Otherwise, a NPC with variable speed, starting position, and characteristics.
 */
character new_character(dungeon_t *d) {
  /* Get number of pre-existing characters */
  int i;
  for (i = 0; d->characters[i].is_alive; i++) {
  }

  /* Assigns character's data fields based on
   * characer type */
  if (i == 0) {
    pc *pc = pc::instance();
    point_t character_position = get_valid_point(d, true);
    pc->position.x = character_position.x;
    pc->position.y = character_position.y;

    pc->sequence_num = i;
    pc->is_alive = true;
    pc->speed = PC_SPEED;
    pc->symbol = '@';

    /* Special Case for loading dungeons from disk */
    if (d->pc != NULL) {
      pc->position.x = d->pc->position.x;
      pc->position.y = d->pc->position.y;
      free(d->pc);  // frees temp pointer for loading purposes only
    }

    d->pc = &(d->characters[0]);

    return *pc;
  } else {
    npc npc = monster_description::generate(d);
    npc.sequence_num = i;

    return npc;
  }
}

/* Returns true if there are NPCs still alive in
 * the dungeon
 */
bool npc_exists(dungeon_t *d) {
  int i;
  for (i = 1; i < (d->num_monsters + 1); i++) {
    if (d->characters[i].is_alive) {
      return true;
    }
  }

  return false;
}

/* Assign characteristic classification. Characteristic classes are stored by a
 * 4 bit number: each bit representing a characteristic. All NPCs have a 50%
 * change of having each of the characteristics. (eventually)
 * TODO: Implement move functions for specific characteristics
 */
/*static uint8_t assign_characteristics() {
  uint8_t characteristics = 0x00;

  if (!(rand() % 2)) {
    characteristics += NPC_SMART;
  }

  if (!(rand() % 2)) {
    characteristics += NPC_TELEPATH;
  }

  if (!(rand() % 5)) {
    characteristics += NPC_TUNNEL;
  }

  if (!(rand() % 2)) {
    characteristics += NPC_ERRATIC;
  }

  return characteristics;
}
*/
