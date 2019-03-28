#ifndef EVENT_SIMULATOR_H
#define EVENT_SIMULATOR_H

#include <stdint.h>
#include "character.h"
#include "dungeon.h"

class dungeon_t;
class character_t;

class event_t {
    public:
    uint32_t turn;
    character_t *c;
};

int event_simulator_start(dungeon_t *);
int event_simulator_stop(dungeon_t *);

#endif
