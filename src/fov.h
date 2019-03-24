#ifndef FOV_H
#define FOV_H

#define FOV_SIGHT_RADIUS 5

#define IS_OPAQUE(terrain) \
    ((terrain == ter_wall || terrain == ter_wall_immutable) ? 1 : 0)

typedef struct dungeon dungeon_t;

void FOV_recursive_shadowcast(dungeon_t *, double, double, int, int, int, int);
#endif
