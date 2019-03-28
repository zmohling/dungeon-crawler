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
