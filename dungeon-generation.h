#ifndef DUNGEON_GEN
#define DUNGEON_GEN

#include <stdbool.h>
#include "dungeon.h"

int generate(dungeon_t *);
int generate_border(dungeon_t *);
int generate_rooms(dungeon_t *);
int generate_corridors(dungeon_t *);
int generate_staircases(dungeon_t *);
int generate_hardness(dungeon_t *);
int generate_terrain(dungeon_t *);
bool intersects(room_t *, room_t *);
bool out_of_bounds(room_t *, int, int);

#endif
