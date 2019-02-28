#include "event_simulator.h"
#include <stdlib.h>
#include "dungeon.h"

static int32_t event_compare(const void *key, const void *with) {
    if ((((event_t *)key)->turn) == (((event_t *)with)->turn)) {
        return ((int32_t)(((event_t *)key)->c->sequence_num) -
                ((int32_t)(((event_t *)with)->c->sequence_num)));
    } else {
        return ((int32_t)(((event_t *)key)->turn) -
                ((int32_t)(((event_t *)with)->turn)));
    }
}

static event_t new_event(character_t *c) {
    event_t e;
    e.turn = 0;
    e.c = c;

    return e;
}

static int game_loop(dungeon_t *d) {
    while ()

    return 0;
}

int event_simulator_start(dungeon_t *d) {

    heap_init(&(d->event_queue), event_compare, NULL);
    d->events = malloc(sizeof(event_t) * d->num_monsters);

    int i;
    for (i = 0; i < d->num_monsters; i++) {
        d->characters[i] = character_add(d);
        d->events[i] = new_event(&(d->characters[i]));
        heap_insert(&(d->event_queue), &(d->events[i]));
    }

    return 0;
}
