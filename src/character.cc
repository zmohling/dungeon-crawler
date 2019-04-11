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

/* Generates and returns a new character. If it's the first character in
 * the dungeon's characters array, will generate a PC character.
 * Otherwise, a NPC with variable speed, starting position, and characteristics.
 */
character_t new_character(dungeon_t *d) {
  character_t c;

  /* Get number of pre-existing characters */
  int i;
  for (i = 0; d->characters[i].is_alive; i++)
    ;
  c.sequence_num = i;

  /* Assigns character's data fields based on
   * characer type */
  if (c.sequence_num == 0) {
    point_t character_position = get_valid_point(d, true);
    c.position.x = character_position.x;
    c.position.y = character_position.y;

    c.is_pc = true;
    c.is_alive = true;
    c.speed = PC_SPEED;
    c.symbol = '@';

    /* Special Case for loading dungeons from disk */
    if (d->pc != NULL) {
      c.position.x = d->pc->position.x;
      c.position.y = d->pc->position.y;
      free(d->pc);  // frees temp pointer for loading purposes only
    }

    d->pc = &(d->characters[0]);
  } else {
    int temp = c.sequence_num;
    c = monster_description::generate(d);
    c.sequence_num = temp;
  }

  return c;
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
