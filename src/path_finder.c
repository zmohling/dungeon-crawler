#include "path_finder.h"

#include <stdlib.h>

#include "dungeon.h"

/* Static dungeon var for comparators */
static dungeon_t *d_static;
static int32_t non_tunnel_compare(const void *key, const void *with) {
    return ((int32_t)d_static
                ->non_tunnel_distance_map[((graph_node_t *)key)->position.y]
                                         [((graph_node_t *)key)->position.x] -
            (int32_t)d_static
                ->non_tunnel_distance_map[((graph_node_t *)with)->position.y]
                                         [((graph_node_t *)with)->position.x]);
}
static int32_t tunnel_compare(const void *key, const void *with) {
    return (
        (int32_t)
            d_static->tunnel_distance_map[((graph_node_t *)key)->position.y]
                                         [((graph_node_t *)key)->position.x] -
        (int32_t)
            d_static->tunnel_distance_map[((graph_node_t *)with)->position.y]
                                         [((graph_node_t *)with)->position.x]);
}

/*
 * Write a distance map for non-tunneling monsters
 * to dungeon_t *d with Dijkstra's Algorithm.
 */
void non_tunnel_distance_map(dungeon_t *d) {
    static graph_node_t graph[DUNGEON_Y][DUNGEON_X], *min;
    static uint32_t initialized = 0;
    uint32_t x, y;
    heap_t h;

    /* Initialize node position and static dungeon */
    if (!initialized) {
        initialized = 1;
        d_static = d;
        for (y = 0; y < DUNGEON_Y; y++) {
            for (x = 0; x < DUNGEON_X; x++) {
                graph[y][x].position.y = y;
                graph[y][x].position.x = x;
            }
        }
    }

    /* Set "cost" to maximum value */
    for (y = 0; y < DUNGEON_Y; y++) {
        for (x = 0; x < DUNGEON_X; x++) {
            d->non_tunnel_distance_map[y][x] = UINT8_MAX;
        }
    }

    /* PC location is of "cost" zero. */
    d->non_tunnel_distance_map[d->pc->position.y][d->pc->position.x] = 0;

    heap_init(&h, non_tunnel_compare, NULL);

    /* Insert all graph_node_ts into heap */
    for (y = 0; y < DUNGEON_Y; y++) {
        for (x = 0; x < DUNGEON_X; x++) {
            if (!IS_SOLID(mapxy(x, y))) {
                graph[y][x].hn = heap_insert(&h, &graph[y][x]);
            }
        }
    }

    while ((min = heap_remove_min(&h))) {
        min->hn = NULL;
        if ((graph[min->position.y - 1][min->position.x - 1].hn) &&
            (d->non_tunnel_distance_map[min->position.y - 1]
                                       [min->position.x - 1] >
             d->non_tunnel_distance_map[min->position.y][min->position.x] +
                 1)) {
            d->non_tunnel_distance_map[min->position.y - 1]
                                      [min->position.x - 1] =
                d->non_tunnel_distance_map[min->position.y][min->position.x] +
                1;
            heap_decrease_key_no_replace(
                &h, graph[min->position.y - 1][min->position.x - 1].hn);
        }
        if ((graph[min->position.y - 1][min->position.x].hn) &&
            (d->non_tunnel_distance_map[min->position.y - 1][min->position.x] >
             d->non_tunnel_distance_map[min->position.y][min->position.x] +
                 1)) {
            d->non_tunnel_distance_map[min->position.y - 1][min->position.x] =
                d->non_tunnel_distance_map[min->position.y][min->position.x] +
                1;
            heap_decrease_key_no_replace(
                &h, graph[min->position.y - 1][min->position.x].hn);
        }
        if ((graph[min->position.y - 1][min->position.x + 1].hn) &&
            (d->non_tunnel_distance_map[min->position.y - 1]
                                       [min->position.x + 1] >
             d->non_tunnel_distance_map[min->position.y][min->position.x] +
                 1)) {
            d->non_tunnel_distance_map[min->position.y - 1]
                                      [min->position.x + 1] =
                d->non_tunnel_distance_map[min->position.y][min->position.x] +
                1;
            heap_decrease_key_no_replace(
                &h, graph[min->position.y - 1][min->position.x + 1].hn);
        }
        if ((graph[min->position.y][min->position.x - 1].hn) &&
            (d->non_tunnel_distance_map[min->position.y][min->position.x - 1] >
             d->non_tunnel_distance_map[min->position.y][min->position.x] +
                 1)) {
            d->non_tunnel_distance_map[min->position.y][min->position.x - 1] =
                d->non_tunnel_distance_map[min->position.y][min->position.x] +
                1;
            heap_decrease_key_no_replace(
                &h, graph[min->position.y][min->position.x - 1].hn);
        }
        if ((graph[min->position.y][min->position.x + 1].hn) &&
            (d->non_tunnel_distance_map[min->position.y][min->position.x + 1] >
             d->non_tunnel_distance_map[min->position.y][min->position.x] +
                 1)) {
            d->non_tunnel_distance_map[min->position.y][min->position.x + 1] =
                d->non_tunnel_distance_map[min->position.y][min->position.x] +
                1;
            heap_decrease_key_no_replace(
                &h, graph[min->position.y][min->position.x + 1].hn);
        }
        if ((graph[min->position.y + 1][min->position.x - 1].hn) &&
            (d->non_tunnel_distance_map[min->position.y + 1]
                                       [min->position.x - 1] >
             d->non_tunnel_distance_map[min->position.y][min->position.x] +
                 1)) {
            d->non_tunnel_distance_map[min->position.y + 1]
                                      [min->position.x - 1] =
                d->non_tunnel_distance_map[min->position.y][min->position.x] +
                1;
            heap_decrease_key_no_replace(
                &h, graph[min->position.y + 1][min->position.x - 1].hn);
        }
        if ((graph[min->position.y + 1][min->position.x].hn) &&
            (d->non_tunnel_distance_map[min->position.y + 1][min->position.x] >
             d->non_tunnel_distance_map[min->position.y][min->position.x] +
                 1)) {
            d->non_tunnel_distance_map[min->position.y + 1][min->position.x] =
                d->non_tunnel_distance_map[min->position.y][min->position.x] +
                1;
            heap_decrease_key_no_replace(
                &h, graph[min->position.y + 1][min->position.x].hn);
        }
        if ((graph[min->position.y + 1][min->position.x + 1].hn) &&
            (d->non_tunnel_distance_map[min->position.y + 1]
                                       [min->position.x + 1] >
             d->non_tunnel_distance_map[min->position.y][min->position.x] +
                 1)) {
            d->non_tunnel_distance_map[min->position.y + 1]
                                      [min->position.x + 1] =
                d->non_tunnel_distance_map[min->position.y][min->position.x] +
                1;
            heap_decrease_key_no_replace(
                &h, graph[min->position.y + 1][min->position.x + 1].hn);
        }
    }

    heap_delete(&h);
}

/*
 * Write a distance map for tunneling monsters
 * to dungeon_t *d with Dijkstra's Algorithm.
 */
void tunnel_distance_map(dungeon_t *d) {
    static graph_node_t graph[DUNGEON_Y][DUNGEON_X], *min;
    static uint32_t initialized = 0;
    uint32_t x, y;
    heap_t h;

    /* Initialize node positions and static dungeon */
    if (!initialized) {
        initialized = 1;
        d_static = d;
        for (y = 0; y < DUNGEON_Y; y++) {
            for (x = 0; x < DUNGEON_X; x++) {
                graph[y][x].position.y = y;
                graph[y][x].position.x = x;
            }
        }
    }

    /* Set "cost" to maximum value */
    for (y = 0; y < DUNGEON_Y; y++) {
        for (x = 0; x < DUNGEON_X; x++) {
            d->tunnel_distance_map[y][x] = UINT8_MAX;
        }
    }

    /* PC location is of "cost" zero. */
    d->tunnel_distance_map[d->pc->position.y][d->pc->position.x] = 0;

    heap_init(&h, tunnel_compare, NULL);

    /* Insert all graph_node_ts into heap */
    for (y = 0; y < DUNGEON_Y; y++) {
        for (x = 0; x < DUNGEON_X; x++) {
            if (mapxy(x, y) != ter_rock_immutable) {
                graph[y][x].hn = heap_insert(&h, &graph[y][x]);
            }
        }
    }

    while ((min = heap_remove_min(&h))) {
        min->hn = NULL;

        if ((graph[min->position.y - 1][min->position.x - 1].hn) &&
            (d->tunnel_distance_map[min->position.y - 1][min->position.x - 1] >
             d->tunnel_distance_map[min->position.y][min->position.x] +
                 tunnel_cost(min->position.x, min->position.y))) {
            d->tunnel_distance_map[min->position.y - 1][min->position.x - 1] =
                (d->tunnel_distance_map[min->position.y][min->position.x] +
                 tunnel_cost(min->position.x, min->position.y));
            heap_decrease_key_no_replace(
                &h, graph[min->position.y - 1][min->position.x - 1].hn);
        }
        if ((graph[min->position.y - 1][min->position.x].hn) &&
            (d->tunnel_distance_map[min->position.y - 1][min->position.x] >
             d->tunnel_distance_map[min->position.y][min->position.x] +
                 tunnel_cost(min->position.x, min->position.y))) {
            d->tunnel_distance_map[min->position.y - 1][min->position.x] =
                (d->tunnel_distance_map[min->position.y][min->position.x] +
                 tunnel_cost(min->position.x, min->position.y));
            heap_decrease_key_no_replace(
                &h, graph[min->position.y - 1][min->position.x].hn);
        }
        if ((graph[min->position.y - 1][min->position.x + 1].hn) &&
            (d->tunnel_distance_map[min->position.y - 1][min->position.x + 1] >
             d->tunnel_distance_map[min->position.y][min->position.x] +
                 tunnel_cost(min->position.x, min->position.y))) {
            d->tunnel_distance_map[min->position.y - 1][min->position.x + 1] =
                (d->tunnel_distance_map[min->position.y][min->position.x] +
                 tunnel_cost(min->position.x, min->position.y));
            heap_decrease_key_no_replace(
                &h, graph[min->position.y - 1][min->position.x + 1].hn);
        }
        if ((graph[min->position.y][min->position.x - 1].hn) &&
            (d->tunnel_distance_map[min->position.y][min->position.x - 1] >
             d->tunnel_distance_map[min->position.y][min->position.x] +
                 tunnel_cost(min->position.x, min->position.y))) {
            d->tunnel_distance_map[min->position.y][min->position.x - 1] =
                (d->tunnel_distance_map[min->position.y][min->position.x] +
                 tunnel_cost(min->position.x, min->position.y));
            heap_decrease_key_no_replace(
                &h, graph[min->position.y][min->position.x - 1].hn);
        }
        if ((graph[min->position.y][min->position.x + 1].hn) &&
            (d->tunnel_distance_map[min->position.y][min->position.x + 1] >
             d->tunnel_distance_map[min->position.y][min->position.x] +
                 tunnel_cost(min->position.x, min->position.y))) {
            d->tunnel_distance_map[min->position.y][min->position.x + 1] =
                (d->tunnel_distance_map[min->position.y][min->position.x] +
                 tunnel_cost(min->position.x, min->position.y));
            heap_decrease_key_no_replace(
                &h, graph[min->position.y][min->position.x + 1].hn);
        }
        if ((graph[min->position.y + 1][min->position.x - 1].hn) &&
            (d->tunnel_distance_map[min->position.y + 1][min->position.x - 1] >
             d->tunnel_distance_map[min->position.y][min->position.x] +
                 tunnel_cost(min->position.x, min->position.y))) {
            d->tunnel_distance_map[min->position.y + 1][min->position.x - 1] =
                (d->tunnel_distance_map[min->position.y][min->position.x] +
                 tunnel_cost(min->position.x, min->position.y));
            heap_decrease_key_no_replace(
                &h, graph[min->position.y + 1][min->position.x - 1].hn);
        }
        if ((graph[min->position.y + 1][min->position.x].hn) &&
            (d->tunnel_distance_map[min->position.y + 1][min->position.x] >
             d->tunnel_distance_map[min->position.y][min->position.x] +
                 tunnel_cost(min->position.x, min->position.y))) {
            d->tunnel_distance_map[min->position.y + 1][min->position.x] =
                (d->tunnel_distance_map[min->position.y][min->position.x] +
                 tunnel_cost(min->position.x, min->position.y));
            heap_decrease_key_no_replace(
                &h, graph[min->position.y + 1][min->position.x].hn);
        }
        if ((graph[min->position.y + 1][min->position.x + 1].hn) &&
            (d->tunnel_distance_map[min->position.y + 1][min->position.x + 1] >
             d->tunnel_distance_map[min->position.y][min->position.x] +
                 tunnel_cost(min->position.x, min->position.y))) {
            d->tunnel_distance_map[min->position.y + 1][min->position.x + 1] =
                (d->tunnel_distance_map[min->position.y][min->position.x] +
                 tunnel_cost(min->position.x, min->position.y));
            heap_decrease_key_no_replace(
                &h, graph[min->position.y + 1][min->position.x + 1].hn);
        }
    }

    heap_delete(&h);
}
