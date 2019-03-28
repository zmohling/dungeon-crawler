#ifndef EVENT_SIMULATOR_H
#define EVENT_SIMULATOR_H

#include <stdint.h>
#include "character.h"

typedef struct event {
    uint32_t turn;
    character_t *c;
} event_t;

int event_simulator_start(dungeon_t *);
int event_simulator_stop(dungeon_t *);

#endif
