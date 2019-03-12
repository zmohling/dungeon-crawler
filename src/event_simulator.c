#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "dungeon.h"
#include "event_simulator.h"
#include "move.h"
#include "path_finder.h"

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
    while (d->pc->is_alive && npc_exists(d)) {
        event_t *e = (event_t *)heap_remove_min(&(d->event_queue));
        if (!e->c->is_alive) {
            continue;
        }
        if (e->c->is_pc) {
            non_tunnel_distance_map(d);
            tunnel_distance_map(d);
            render_dungeon(d);

            getch();

        } else {
            move_npc(d, e->c);
        }

        e->turn += (1000 / e->c->speed);
        heap_insert(&(d->event_queue), e);
    }

    render_dungeon(d);
    printf("GAME OVER\n");

    return 0;
}

int event_simulator_start(dungeon_t *d) {
    d->events = calloc(1, sizeof(event_t) * d->num_monsters);
    d->characters = calloc(
        1, sizeof(character_t) * (d->num_monsters + 1));  // plus one for PC

    heap_init(&(d->event_queue), event_compare, NULL);

    int i;
    for (i = 0; i < (d->num_monsters + 1); i++) {
        d->characters[i] = character_add(d);
        d->character_map[d->characters[i].position.y]
                        [d->characters[i].position.x] = &(d->characters[i]);

        d->events[i] = new_event(&(d->characters[i]));
        heap_insert(&(d->event_queue), &(d->events[i]));
    }

    d->pc = &(d->characters[0]);

    non_tunnel_distance_map(d);
    tunnel_distance_map(d);

    game_loop(d);

    return 0;
}
