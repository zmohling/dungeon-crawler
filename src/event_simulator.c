#include "event_simulator.h"

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "accessory_screens.h"
#include "dungeon.h"
#include "input.h"
#include "move.h"
#include "path_finder.h"
#include "fov.h"

/* Compare function for the discrete event simulator and heap */
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
    /* Init distance maps */
    non_tunnel_distance_map(d);
    tunnel_distance_map(d);
    render_dungeon(d);

    /* Game Loop with discrete event simulator */
    while (d->pc->is_alive && npc_exists(d)) {
        event_t *e = (event_t *)heap_remove_min(&(d->event_queue));

        if (!e->c->is_alive) {
            ;
        } else if (e->c->is_pc) {
            render_dungeon(d);
            FOV_recursive_shadowcast(d, 1.0, 0, d->pc->position.y, d->pc->position.x, 1, 5);

            handle_key(d, getch());
        } else {
            move_npc(d, e->c);
        }

        e->turn += (1000 / e->c->speed);
        heap_insert(&(d->event_queue), e);
    }

    /* Show output for 0.25 seconds */
    render_dungeon(d);
    usleep(250000);

    /* End Game */
    if (d->pc->is_alive) {
        endscreen(1);
    } else {
        endscreen(0);
    }

    return 0;
}

/* Starts the game loop and initializes the discrete event
 * simulator. Populates event heap, characters array, and 
 * character_map fields of the Dungeon struct. Memory dealloc.
 * is in dungeon.c:deep_free_dungeon()
 */
int event_simulator_start(dungeon_t *d) {
    /* Allocate memory for events and characters arrays */
    if (!(d->events = calloc(1, sizeof(event_t) * (d->num_monsters + 1)))) {
        fprintf(stderr, "Did not allocate events array in dungeon struct.");
        exit(-122);
    }
    if (!(d->characters =
              calloc(1, sizeof(character_t) * (d->num_monsters + 1)))) {
        fprintf(stderr, "Did not allocate characters array in dungeon struct.");
        exit(-123);
    }

    heap_init(&(d->event_queue), event_compare, NULL);

    /* Populate arrays, create events, and insert events into heap */
    int i;
    for (i = 0; i < (d->num_monsters + 1); i++) {
        d->characters[i] = new_character(d);
        d->character_map[d->characters[i].position.y]
                        [d->characters[i].position.x] = &(d->characters[i]);

        d->events[i] = new_event(&(d->characters[i]));
        heap_insert(&(d->event_queue), &(d->events[i]));
    }

    /* Dungeon's PC pointer. PC is always first in characters array. */
    //d->pc = &(d->characters[0]);


    game_loop(d);

    return 0;
}

int event_simulator_stop(dungeon_t *d) {

    return 0;
}
