#include "character.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "dungeon.h"

static uint8_t assign_characteristics();

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

    point_t character_position = get_valid_point(d);
        c.position.x = character_position.x;
        c.position.y = character_position.y;

    /* Assigns character's data fields based on 
     * characer type */
    if (c.sequence_num == 0) {
        c.is_pc = true;
        c.is_alive = true;
        c.speed = PC_SPEED;
        c.symbol = '@';

        /* Special Case for loading dungeons from disk */
        if (d->pc != NULL) {
            c.position.x = d->pc->position.x;
            c.position.y = d->pc->position.y;
            free(d->pc); // frees temp pointer for loading purposes only
        }
    } else {
        c.is_pc = false;
        c.is_alive = true;
        c.speed = rand() % (NPC_MAX_SPEED - 4) + 5;

        c.npc = calloc(1, sizeof(npc_t));
        c.npc->characteristics = assign_characteristics();
        c.symbol = (char)c.npc->characteristics;
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
 * 4 bit number: each bit representing a characteristic. All NPCs have a 50% change
 * of having each of the characteristics. (eventually)
 * TODO: Implement move functions for specific characteristics
 */
static uint8_t assign_characteristics() {
    uint8_t characteristics = 0x00;

    if (!(rand() % 2)) {
        characteristics += NPC_INTELLIGENT;
    }

    if (!(rand() % 2)) {
        characteristics += NPC_TELEPATHIC;
    }

    if (!(rand() % 5)) {
        characteristics += NPC_TUNNELING;
    }

    if (!(rand() % 2)) {
        characteristics += NPC_ERRATIC;
    }

    return characteristics;
}
