#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "character.h"

int character_add_npc(dungeon_t *d) {
    static uint8_t sequencer = 0;

    character_t c;
    c.is_alive = true;
    c.speed = rand() % 16 + 5;
    c.sequence_num = sequencer++;

    c.is_pc = false;
    npc_t npc;
    npc.characteristics = assign_characteristics();

    return 0;
}

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
