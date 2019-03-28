#ifndef FOV_H
#define FOV_H

#define FOV_SIGHT_RADIUS 5

#include "stdbool.h"

class dungeon_t;
class point_t;
class slope_pair_t {
    public:
    double start_slope;
    double end_slope;
};

void FOV_recursive_shadowcast(dungeon_t *, point_t *, slope_pair_t, int, int,
                              int);
void FOV_shadowcast(dungeon_t *, point_t *, int);
void FOV_clear(dungeon_t *);
bool FOV_get_fog();
void FOV_toggle_fog();

#endif
