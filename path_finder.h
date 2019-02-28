#ifndef PATHFINDER_H
#define PATHFINDER_H

#include "dungeon.h"
#include "heap.h"

#define mapxy(x, y) (d->map[y][x])
#define tunnel_cost(x, y)             \
    (d->hardness_map[y][x] < (85 * 1) \
         ? 1                          \
         : (d->hardness_map[y][x] < (85 * 2) + 1 ? 2 : 3))

typedef struct graph_node {
    heap_node_t *hn;
    point_t position;
} graph_node_t;

void non_tunnel_distance_map(dungeon_t *d);
void tunnel_distance_map(dungeon_t *d);

#endif
