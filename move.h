#ifndef MOVE_H
#define MOVE_H

#include <stdint.h>
#include "dungeon.h" 

int move_npc(dungeon_t *, character_t *);
int move_npc_non_tunnel(dungeon_t *, character_t *);
int move_npc_tunnel(dungeon_t *, character_t *);
int check_for_trample(dungeon_t *, int, int);

#endif
