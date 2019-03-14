#include "character.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "dungeon.h"

static uint8_t assign_characteristics();

character_t character_add(dungeon_t *d) {
    int i;
    for (i = 0; d->characters[i].is_alive; i++)
        ;

    character_t c;
    c.is_alive = true;
    c.sequence_num = i;

    point_t position = get_valid_point(d);

    if (c.sequence_num == 0) {
        c.is_pc = true;
        c.speed = PC_SPEED;
        c.symbol = '@';

        /* Special Case for loading dungeons from disk */
        if (d->pc == NULL) {
            c.position.x = position.x;
            c.position.y = position.y;
        } else {
            c.position.x = d->pc->position.x;
            c.position.y = d->pc->position.y;
            free(d->pc);
        }
    } else {
        c.is_pc = false;
        c.speed = rand() % 16 + 5;
        c.npc = calloc(1, sizeof(npc_t));
        c.npc->characteristics = assign_characteristics();
        c.symbol = (char) c.npc->characteristics;

        c.position.x = position.x;
        c.position.y = position.y;
    }

    return c;
}

bool npc_exists(dungeon_t *d) {
    int i;
    for (i = 1; i < (d->num_monsters + 1); i++) {
        if (d->characters[i].is_alive) {
            return true;
        }
    }

    return false;
}

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
