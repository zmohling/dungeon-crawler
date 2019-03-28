#ifndef FOV_H
#define FOV_H

#define FOV_SIGHT_RADIUS 5

#include "stdbool.h"

typedef struct dungeon dungeon_t;
typedef struct point point_t;
typedef struct {
    double start_slope;
    double end_slope;
} slope_pair_t;


void FOV_recursive_shadowcast(dungeon_t *, point_t *, slope_pair_t, int, int,
                              int);
void FOV_shadowcast(dungeon_t *, point_t *, int);
void FOV_clear(dungeon_t *);
bool FOV_get_fog();
void FOV_toggle_fog();

#endif
