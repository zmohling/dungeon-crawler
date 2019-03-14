#include <menu.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "accessory_screens.h"
#include "dungeon.h"
#include "event_simulator.h"
#include "input.h"
#include "move.h"
#include "path_finder.h"

static void endscreen(int);
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
    /* Game Loop */
    while (d->pc->is_alive && npc_exists(d)) {
        event_t *e = (event_t *)heap_remove_min(&(d->event_queue));
        if (!e->c->is_alive) {
            continue;
        }
        if (e->c->is_pc) {
            non_tunnel_distance_map(d);
            tunnel_distance_map(d);
            render_dungeon(d);

            handle_key(d, getch());
        } else {
            move_npc(d, e->c);
        }

        e->turn += (1000 / e->c->speed);
        heap_insert(&(d->event_queue), e);
    }

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

int event_simulator_start(dungeon_t *d) {
    if (!(d->events = calloc(1, sizeof(event_t) * (d->num_monsters + 1)))) {
        fprintf(stderr, "Did not allocate events array in dungeon struct.");
        exit(-122);
    }
    if (!(d->characters =
              calloc(1, sizeof(character_t) * (d->num_monsters + 1)))) {
        fprintf(stderr, "Did not allocate events array in dungeon struct.");
        exit(-122);
    }

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

static void endscreen(int didWin) {
    WINDOW *local_win;

    local_win = newwin(23, 81, 1, 0);
    box(local_win, 0, 0);
    wrefresh(local_win);

    char variableStr[81];
    if (didWin) {
        strncpy(variableStr, "<   You Won!   >", 81);
    } else {
        strncpy(variableStr, "< You've Died! >", 81);
    }

    mvprintw(0, 0, "                     ");
    mvprintw(5, 25, " ______________ ");
    mvprintw(6, 25, "%s", variableStr);
    mvprintw(7, 25, " -------------- ");
    mvprintw(8, 25, "        \\   ^__^");
    mvprintw(9, 25, "         \\  (oo)\\_______");
    mvprintw(10, 25, "            (__)\\       )\\/\\");
    mvprintw(11, 25, "                ||----w |");
    mvprintw(12, 25, "                ||     ||");
    mvprintw(18, 25, "PRESS ANY CHARACTER TO EXIT");

    getch();

    endwin();
    exit(0);
}
