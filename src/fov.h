#ifndef FOV_H
#define FOV_H

#define FOV_SIGHT_RADIUS 5

#define IS_OPAQUE(terrain) \
    ((terrain == ter_wall || terrain == ter_wall_immutable || terrain == ter_stairs_up || terrain == ter_stairs_down) ? 1 : 0)

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

#endif
