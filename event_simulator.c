#include <stdlib.h>
#include "dungeon.h"

int event_simulator_start(dungeon_t *d) {

    int i;
    for (i = 0; i < d->num_monsters; i++) {
        character_add_npc(d);
    }


    return 0;
}

static int game_loop(dungeon_t *d) {


    return 0;
}


