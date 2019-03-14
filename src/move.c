#include <limits.h>
#include <math.h>
#include <ncurses.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "dungeon.h"
#include "geometry.h"
#include "input.h"
#include "move.h"
#include "path_finder.h"

int move_pc(dungeon_t *d, int c) {
    character_t *pc = d->pc;
    point_t next_pos;

    next_pos.x = pc->position.x;
    next_pos.y = pc->position.y;

    switch (c) {
        case '7':
        case 'y':
            next_pos.x = next_pos.x - 1;
            next_pos.y = next_pos.y - 1;
            break;
        case '8':
        case 'k':
            next_pos.x = next_pos.x;
            next_pos.y = next_pos.y - 1;
            break;
        case '9':
        case 'u':
            next_pos.x = next_pos.x + 1;
            next_pos.y = next_pos.y - 1;
            break;
        case '6':
        case 'l':
            next_pos.x = next_pos.x + 1;
            next_pos.y = next_pos.y;
            break;
        case '3':
        case 'n':
            next_pos.x = next_pos.x + 1;
            next_pos.y = next_pos.y + 1;
            break;
        case '2':
        case 'j':
            next_pos.x = next_pos.x;
            next_pos.y = next_pos.y + 1;
            break;
        case '1':
        case 'b':
            next_pos.x = next_pos.x - 1;
            next_pos.y = next_pos.y + 1;
            break;
        case '4':
        case 'h':
            next_pos.x = next_pos.x - 1;
            next_pos.y = next_pos.y;
            break;
        case '5':
        case '.':
        case ' ':
            return 0;
        default:
            next_pos.x = 0;
            next_pos.y = 0;
            break;
    }

    if (d->map[next_pos.y][next_pos.x] != ter_wall &&
        d->map[next_pos.y][next_pos.x] != ter_wall_immutable) {

        if(!(check_for_trample(d, next_pos.x, next_pos.y))) {
            mvprintw(0, 0, "You slayed a monster!");
        }
        d->character_map[pc->position.y][pc->position.x] = NULL;
        pc->position = next_pos;
        d->character_map[pc->position.y][pc->position.x] = pc;

        return 0;
    } else {
        return -1;
    }
}

int use_stairs(dungeon_t *d, pc_movement_t p) {
    deep_free_dungeon(d);
    generate_dungeon(d);
    event_simulator_start(d);

    return 0;
}

int move_npc(dungeon_t *d, character_t *c) {
    if (c->npc->characteristics & 0x04) {
        move_npc_tunnel(d, c);
    } else {
        move_npc_non_tunnel(d, c);
    }

    return 0;
}

int move_npc_non_tunnel(dungeon_t *d, character_t *c) {
    point_t next_pos;
    next_pos.x = c->position.x;
    next_pos.y = c->position.y;

    uint8_t distance = UINT8_MAX;

    int x, y;
    for (y = (c->position.y - 1); y <= (c->position.y + 1); y++) {
        for (x = (c->position.x - 1); x <= (c->position.x + 1); x++) {
            if (y > (DUNGEON_Y - 1) || y < 0 || x > (DUNGEON_X - 1) || x < 0) {
                continue;
            } else if (d->map[y][x] == ter_wall ||
                       d->map[y][x] == ter_wall_immutable) {
                continue;
            }

            if (d->non_tunnel_distance_map[y][x] < distance ||
                (d->non_tunnel_distance_map[y][x] <= distance &&
                 ((abs(x - c->position.x) + abs(y - c->position.y)) == 1))) {
                distance = d->non_tunnel_distance_map[y][x];
                next_pos.x = x;
                next_pos.y = y;
            }
        }
    }

    check_for_trample(d, next_pos.x, next_pos.y);
    d->character_map[c->position.y][c->position.x] = NULL;
    c->position = next_pos;
    d->character_map[c->position.y][c->position.x] = c;

    return 0;
}

int move_npc_tunnel(dungeon_t *d, character_t *c) {
    point_t next_pos;
    next_pos.x = c->position.x;
    next_pos.y = c->position.y;
    uint8_t min_cost = UINT8_MAX;

    int x, y;
    for (y = (c->position.y - 1); y <= (c->position.y + 1); y++) {
        for (x = (c->position.x - 1); x <= (c->position.x + 1); x++) {
            if (y > (DUNGEON_Y - 1) || y < 0 || x > (DUNGEON_X - 1) || x < 0) {
                continue;
            } else if (d->map[y][x] == ter_wall_immutable) {
                continue;
            }

            uint8_t cost =
                d->tunnel_distance_map[y][x] + (d->hardness_map[y][x] / 85);

            if (cost < min_cost ||
                (cost <= min_cost &&
                 ((abs(x - c->position.x) + abs(y - c->position.y)) == 1))) {
                min_cost = cost;
                next_pos.x = x;
                next_pos.y = y;
            }
        }
    }
    if (d->hardness_map[next_pos.y][next_pos.x] > 85) {
        d->hardness_map[next_pos.y][next_pos.x] -= 85;
        tunnel_distance_map(d);

        return 0;
    } else if (d->hardness_map[next_pos.y][next_pos.x] <= 85 &&
               d->hardness_map[next_pos.y][next_pos.x] > 0) {
        d->hardness_map[next_pos.y][next_pos.x] = 0;
        d->map[next_pos.y][next_pos.x] = ter_floor_hall;

        non_tunnel_distance_map(d);
        tunnel_distance_map(d);
    }
    if (d->hardness_map[next_pos.y][next_pos.x] == 0) {
        check_for_trample(d, next_pos.x, next_pos.y);
        d->character_map[c->position.y][c->position.x] = NULL;
        c->position.x = next_pos.x;
        c->position.y = next_pos.y;
        d->character_map[c->position.y][c->position.x] = c;
    }
    return 0;
}

int check_for_trample(dungeon_t *d, int x, int y) {
    if (d->character_map[y][x] != NULL && d->character_map[y][x]->is_alive) {
        d->character_map[y][x]->is_alive = false;
        d->character_map[y][x] = NULL;

        return 0;
    }

    return 1;  // no kill
}
