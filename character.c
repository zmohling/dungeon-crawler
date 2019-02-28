#include "character.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "dungeon.h"

static uint8_t assign_characteristics();

character_t *character_add_npc(dungeon_t *d) {
    static uint8_t sequencer = 0;

    point_t position = get_valid_point(d);
    d->character_map[position.y][position.x] = malloc(sizeof(character_t));
    character_t *c = d->character_map[position.y][position.x];

        c->is_alive = true;
        c->sequence_num = sequencer++;
        c->position.x = position.x;
        c->position.y = position.y;

    if (sequencer == 0) {
        c->is_pc = true;
        c->speed = PC_SPEED;
        c->symbol = '@';
    } else {
        c->is_pc = false;
        c->speed = rand() % 16 + 5;
        c->npc = malloc(sizeof(npc_t));
        c->npc->characteristics = assign_characteristics();
        c->symbol = (char)c->npc->characteristics;
    }

    return c;
}

bool npc_exists(dungeon_t *d) {}

static uint8_t assign_characteristics() {
    uint8_t characteristics = 0x00;

    if (rand() % 2) {
        characteristics += NPC_INTELLIGENT;
    }

    if (rand() % 2) {
        characteristics += NPC_TELEPATHIC;
    }

    if (rand() % 2) {
        characteristics += NPC_TUNNELING;
    }

    if (rand() % 2) {
        characteristics += NPC_ERRATIC;
    }

    return characteristics;
}
