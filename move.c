#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "dungeon.h"
#include "geometry.h"
#include "path_finder.h"

#include "move.h"
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

    d->character_map[c->position.y][c->position.x] = NULL;
    check_for_trample(d, next_pos.x, next_pos.y);
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
        d->character_map[c->position.y][c->position.x] = NULL;
        check_for_trample(d, next_pos.x, next_pos.y);
        c->position.x = next_pos.x;
        c->position.y = next_pos.y;
        d->character_map[c->position.y][c->position.x] = c;
    }
    return 0;
}

int check_for_trample(dungeon_t *d, int x, int y) {
    if (d->character_map[y][x] != NULL) {
        d->character_map[y][x]->is_alive = false;
        d->character_map[y][x] = NULL;

        return 0;
    }

    return 1;  // no kill
}
