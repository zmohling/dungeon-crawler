#ifndef CHARACTER_H
#define CHARACTER_H

#include <stdint.h>
#include <stdbool.h>
#include "geometry.h"

#define NPC_INTELLIGENT 0x01
#define NPC_TELEPATHIC  0x02
#define NPC_TUNNELING   0x04
#define NPC_ERRATIC     0x08
#define PC              0x80

typedef struct dungeon dungeon_t;

typedef struct npc {
    int8_t characteristics;

    bool has_seen_pc;
    point_t pc_position;
} npc_t;

typedef struct character {
    char symbol;
    bool is_alive;
    int8_t speed;
    int8_t sequence_num;

    bool is_pc;
    npc_t *npc;

    point_t position;
} character_t;

character_t * character_add_npc(dungeon_t *);

#endif
