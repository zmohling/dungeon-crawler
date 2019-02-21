#include "pathfinder.h"
#include <stdlib.h>
#include "dungeon.h"

/* Static dungeon var for comparators */
static dungeon_t *d_static;
static int32_t non_tunnel_compare(const void *key, const void *with) {
    return (
        (int32_t)
            d_static->non_tunnel_distance_map[((graph_node_t *)key)->pos.y]
                                             [((graph_node_t *)key)->pos.x] -
        (int32_t)
            d_static->non_tunnel_distance_map[((graph_node_t *)with)->pos.y]
                                             [((graph_node_t *)with)->pos.x]);
}
static int32_t tunnel_compare(const void *key, const void *with) {
    return (
        (int32_t)d_static->tunnel_distance_map[((graph_node_t *)key)->pos.y]
                                              [((graph_node_t *)key)->pos.x] -
        (int32_t)d_static->tunnel_distance_map[((graph_node_t *)with)->pos.y]
                                              [((graph_node_t *)with)->pos.x]);
}

/*
 * Write a distance map for non-tunneling monsters
 * to dungeon_t *d
 */
void non_tunnel_distance_map(dungeon_t *d) {
    static graph_node_t graph[DUNGEON_Y][DUNGEON_X], *min;
    static uint32_t initialized = 0;
    uint32_t x, y;
    heap_t h;

    if (!initialized) {
        initialized = 1;
        d_static = d;
        for (y = 0; y < DUNGEON_Y; y++) {
            for (x = 0; x < DUNGEON_X; x++) {
                graph[y][x].pos.y = y;
                graph[y][x].pos.x = x;
            }
        }
    }

    for (y = 0; y < DUNGEON_Y; y++) {
        for (x = 0; x < DUNGEON_X; x++) {
            d->non_tunnel_distance_map[y][x] = UINT8_MAX;
        }
    }
    d->non_tunnel_distance_map[d->pc_coordinates.y][d->pc_coordinates.x] = 0;

    heap_init(&h, non_tunnel_compare, NULL);

    for (y = 0; y < DUNGEON_Y; y++) {
        for (x = 0; x < DUNGEON_X; x++) {
            if (mapxy(x, y) != ter_wall && mapxy(x, y) != ter_wall_immutable) {
                graph[y][x].hn = heap_insert(&h, &graph[y][x]);
            }
        }
    }

    while ((min = heap_remove_min(&h))) {
        min->hn = NULL;
        if ((graph[min->pos.y - 1][min->pos.x - 1].hn) &&
            (d->non_tunnel_distance_map[min->pos.y - 1][min->pos.x - 1] >
             d->non_tunnel_distance_map[min->pos.y][min->pos.x] + 1)) {
            d->non_tunnel_distance_map[min->pos.y - 1][min->pos.x - 1] =
                d->non_tunnel_distance_map[min->pos.y][min->pos.x] + 1;
            heap_decrease_key_no_replace(
                &h, graph[min->pos.y - 1][min->pos.x - 1].hn);
        }
        if ((graph[min->pos.y - 1][min->pos.x].hn) &&
            (d->non_tunnel_distance_map[min->pos.y - 1][min->pos.x] >
             d->non_tunnel_distance_map[min->pos.y][min->pos.x] + 1)) {
            d->non_tunnel_distance_map[min->pos.y - 1][min->pos.x] =
                d->non_tunnel_distance_map[min->pos.y][min->pos.x] + 1;
            heap_decrease_key_no_replace(&h,
                                         graph[min->pos.y - 1][min->pos.x].hn);
        }
        if ((graph[min->pos.y - 1][min->pos.x + 1].hn) &&
            (d->non_tunnel_distance_map[min->pos.y - 1][min->pos.x + 1] >
             d->non_tunnel_distance_map[min->pos.y][min->pos.x] + 1)) {
            d->non_tunnel_distance_map[min->pos.y - 1][min->pos.x + 1] =
                d->non_tunnel_distance_map[min->pos.y][min->pos.x] + 1;
            heap_decrease_key_no_replace(
                &h, graph[min->pos.y - 1][min->pos.x + 1].hn);
        }
        if ((graph[min->pos.y][min->pos.x - 1].hn) &&
            (d->non_tunnel_distance_map[min->pos.y][min->pos.x - 1] >
             d->non_tunnel_distance_map[min->pos.y][min->pos.x] + 1)) {
            d->non_tunnel_distance_map[min->pos.y][min->pos.x - 1] =
                d->non_tunnel_distance_map[min->pos.y][min->pos.x] + 1;
            heap_decrease_key_no_replace(&h,
                                         graph[min->pos.y][min->pos.x - 1].hn);
        }
        if ((graph[min->pos.y][min->pos.x + 1].hn) &&
            (d->non_tunnel_distance_map[min->pos.y][min->pos.x + 1] >
             d->non_tunnel_distance_map[min->pos.y][min->pos.x] + 1)) {
            d->non_tunnel_distance_map[min->pos.y][min->pos.x + 1] =
                d->non_tunnel_distance_map[min->pos.y][min->pos.x] + 1;
            heap_decrease_key_no_replace(&h,
                                         graph[min->pos.y][min->pos.x + 1].hn);
        }
        if ((graph[min->pos.y + 1][min->pos.x - 1].hn) &&
            (d->non_tunnel_distance_map[min->pos.y + 1][min->pos.x - 1] >
             d->non_tunnel_distance_map[min->pos.y][min->pos.x] + 1)) {
            d->non_tunnel_distance_map[min->pos.y + 1][min->pos.x - 1] =
                d->non_tunnel_distance_map[min->pos.y][min->pos.x] + 1;
            heap_decrease_key_no_replace(
                &h, graph[min->pos.y + 1][min->pos.x - 1].hn);
        }
        if ((graph[min->pos.y + 1][min->pos.x].hn) &&
            (d->non_tunnel_distance_map[min->pos.y + 1][min->pos.x] >
             d->non_tunnel_distance_map[min->pos.y][min->pos.x] + 1)) {
            d->non_tunnel_distance_map[min->pos.y + 1][min->pos.x] =
                d->non_tunnel_distance_map[min->pos.y][min->pos.x] + 1;
            heap_decrease_key_no_replace(&h,
                                         graph[min->pos.y + 1][min->pos.x].hn);
        }
        if ((graph[min->pos.y + 1][min->pos.x + 1].hn) &&
            (d->non_tunnel_distance_map[min->pos.y + 1][min->pos.x + 1] >
             d->non_tunnel_distance_map[min->pos.y][min->pos.x] + 1)) {
            d->non_tunnel_distance_map[min->pos.y + 1][min->pos.x + 1] =
                d->non_tunnel_distance_map[min->pos.y][min->pos.x] + 1;
            heap_decrease_key_no_replace(
                &h, graph[min->pos.y + 1][min->pos.x + 1].hn);
        }
    }
    heap_delete(&h);
}

void tunnel_distance_map(dungeon_t *d) {
    heap_t h;
    uint32_t x, y;
    int size;
    static graph_node_t graph[DUNGEON_Y][DUNGEON_X], *min;
    static uint32_t initialized = 0;

    if (!initialized) {
        initialized = 1;
        d_static = d;
        for (y = 0; y < DUNGEON_Y; y++) {
            for (x = 0; x < DUNGEON_X; x++) {
                graph[y][x].pos.y = y;
                graph[y][x].pos.x = x;
            }
        }
    }

    for (y = 0; y < DUNGEON_Y; y++) {
        for (x = 0; x < DUNGEON_X; x++) {
            d->tunnel_distance_map[y][x] = UINT8_MAX;
        }
    }
    d->tunnel_distance_map[d->pc_coordinates.y][d->pc_coordinates.x] = 0;

    heap_init(&h, tunnel_compare, NULL);

    for (y = 0; y < DUNGEON_Y; y++) {
        for (x = 0; x < DUNGEON_X; x++) {
            if (mapxy(x, y) != ter_wall_immutable) {
                graph[y][x].hn = heap_insert(&h, &graph[y][x]);
            }
        }
    }

    size = h.size;
    while ((min = heap_remove_min(&h))) {
        if (--size != h.size) {
            exit(1);
        }
        min->hn = NULL;
        if ((graph[min->pos.y - 1][min->pos.x - 1].hn) &&
            (d->tunnel_distance_map[min->pos.y - 1][min->pos.x - 1] >
             d->tunnel_distance_map[min->pos.y][min->pos.x] +
                 tunnel_cost(min->pos.x, min->pos.y))) {
            d->tunnel_distance_map[min->pos.y - 1][min->pos.x - 1] =
                (d->tunnel_distance_map[min->pos.y][min->pos.x] +
                 tunnel_cost(min->pos.x, min->pos.y));
            heap_decrease_key_no_replace(
                &h, graph[min->pos.y - 1][min->pos.x - 1].hn);
        }
        if ((graph[min->pos.y - 1][min->pos.x].hn) &&
            (d->tunnel_distance_map[min->pos.y - 1][min->pos.x] >
             d->tunnel_distance_map[min->pos.y][min->pos.x] +
                 tunnel_cost(min->pos.x, min->pos.y))) {
            d->tunnel_distance_map[min->pos.y - 1][min->pos.x] =
                (d->tunnel_distance_map[min->pos.y][min->pos.x] +
                 tunnel_cost(min->pos.x, min->pos.y));
            heap_decrease_key_no_replace(&h,
                                         graph[min->pos.y - 1][min->pos.x].hn);
        }
        if ((graph[min->pos.y - 1][min->pos.x + 1].hn) &&
            (d->tunnel_distance_map[min->pos.y - 1][min->pos.x + 1] >
             d->tunnel_distance_map[min->pos.y][min->pos.x] +
                 tunnel_cost(min->pos.x, min->pos.y))) {
            d->tunnel_distance_map[min->pos.y - 1][min->pos.x + 1] =
                (d->tunnel_distance_map[min->pos.y][min->pos.x] +
                 tunnel_cost(min->pos.x, min->pos.y));
            heap_decrease_key_no_replace(
                &h, graph[min->pos.y - 1][min->pos.x + 1].hn);
        }
        if ((graph[min->pos.y][min->pos.x - 1].hn) &&
            (d->tunnel_distance_map[min->pos.y][min->pos.x - 1] >
             d->tunnel_distance_map[min->pos.y][min->pos.x] +
                 tunnel_cost(min->pos.x, min->pos.y))) {
            d->tunnel_distance_map[min->pos.y][min->pos.x - 1] =
                (d->tunnel_distance_map[min->pos.y][min->pos.x] +
                 tunnel_cost(min->pos.x, min->pos.y));
            heap_decrease_key_no_replace(&h,
                                         graph[min->pos.y][min->pos.x - 1].hn);
        }
        if ((graph[min->pos.y][min->pos.x + 1].hn) &&
            (d->tunnel_distance_map[min->pos.y][min->pos.x + 1] >
             d->tunnel_distance_map[min->pos.y][min->pos.x] +
                 tunnel_cost(min->pos.x, min->pos.y))) {
            d->tunnel_distance_map[min->pos.y][min->pos.x + 1] =
                (d->tunnel_distance_map[min->pos.y][min->pos.x] +
                 tunnel_cost(min->pos.x, min->pos.y));
            heap_decrease_key_no_replace(&h,
                                         graph[min->pos.y][min->pos.x + 1].hn);
        }
        if ((graph[min->pos.y + 1][min->pos.x - 1].hn) &&
            (d->tunnel_distance_map[min->pos.y + 1][min->pos.x - 1] >
             d->tunnel_distance_map[min->pos.y][min->pos.x] +
                 tunnel_cost(min->pos.x, min->pos.y))) {
            d->tunnel_distance_map[min->pos.y + 1][min->pos.x - 1] =
                (d->tunnel_distance_map[min->pos.y][min->pos.x] +
                 tunnel_cost(min->pos.x, min->pos.y));
            heap_decrease_key_no_replace(
                &h, graph[min->pos.y + 1][min->pos.x - 1].hn);
        }
        if ((graph[min->pos.y + 1][min->pos.x].hn) &&
            (d->tunnel_distance_map[min->pos.y + 1][min->pos.x] >
             d->tunnel_distance_map[min->pos.y][min->pos.x] +
                 tunnel_cost(min->pos.x, min->pos.y))) {
            d->tunnel_distance_map[min->pos.y + 1][min->pos.x] =
                (d->tunnel_distance_map[min->pos.y][min->pos.x] +
                 tunnel_cost(min->pos.x, min->pos.y));
            heap_decrease_key_no_replace(&h,
                                         graph[min->pos.y + 1][min->pos.x].hn);
        }
        if ((graph[min->pos.y + 1][min->pos.x + 1].hn) &&
            (d->tunnel_distance_map[min->pos.y + 1][min->pos.x + 1] >
             d->tunnel_distance_map[min->pos.y][min->pos.x] +
                 tunnel_cost(min->pos.x, min->pos.y))) {
            d->tunnel_distance_map[min->pos.y + 1][min->pos.x + 1] =
                (d->tunnel_distance_map[min->pos.y][min->pos.x] +
                 tunnel_cost(min->pos.x, min->pos.y));
            heap_decrease_key_no_replace(
                &h, graph[min->pos.y + 1][min->pos.x + 1].hn);
        }
    }
    heap_delete(&h);
}
