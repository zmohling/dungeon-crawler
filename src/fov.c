#include "fov.h"

#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "dungeon.h"
#include "geometry.h"

static double FOV_slope(double x1, double y1, double x2, double y2,
                        int octant) {
    double result = 0;

    switch (octant) {
        case 1:
        case 5:
            result =
                -1 * (((y2 - 0.5) - (y1 - 0.5)) / ((x2 - 0.5) - (x1 - 0.5)));
            break;
        case 4:
        case 8:
            result =
                1 * (((y2 - 0.5) - (y1 - 0.5)) / ((x2 - 0.5) - (x1 - 0.5)));
            break;
        case 3:
        case 7:
            result =
                1 / (((y2 - 0.5) - (y1 - 0.5)) / ((x2 - 0.5) - (x1 - 0.5)));
            break;
        case 2:
        case 6:
            result =
                -1 / (((y2 - 0.5) - (y1 - 0.5)) / ((x2 - 0.5) - (x1 - 0.5)));
            break;
    }

    if (isnan(result) || isinf(result)) {
        return 0.0;
    } else {
        return ((double)roundf(result * 100) / 100);
    }
}

static void FOV_get_next_cell(point_t *p, int octant) {
    switch (octant) {
        case 1:
        case 4:
            p->y++;
            break;
        case 2:
        case 7:
            p->x--;
            break;
        case 3:
        case 6:
            p->x++;
            break;
        case 5:
        case 8:
            p->y--;
            break;
    }
}

static point_t FOV_get_prev_cell(point_t p, int octant) {
    point_t prev_cell = p;
    FOV_get_next_cell(&p, octant);

    prev_cell.x += (-1) * (p.x - prev_cell.x);
    prev_cell.y += (-1) * (p.y - prev_cell.y);

    return prev_cell;
}

/* src is pc position */
static point_t FOV_get_starting_cell(point_t *src, int units_from_src,
                                     int octant) {
    point_t p = *src;

    switch (octant) {
        case 1:
        case 2:
            p.x += units_from_src;
            p.y -= units_from_src;
            break;
        case 3:
        case 4:
            p.x -= units_from_src;
            p.y -= units_from_src;
            break;
        case 5:
        case 6:
            p.x -= units_from_src;
            p.y += units_from_src;
            break;
        case 7:
        case 8:
            p.x += units_from_src;
            p.y += units_from_src;
            break;
    }

    return p;
}

static int FOV_in_arch(double slope, slope_pair_t arch, int octant) {
    return (slope <= arch.start_slope && slope >= arch.end_slope);
}

void FOV_recursive_shadowcast(dungeon_t *d, point_t *origin,
                              slope_pair_t slopes, int offset, int radius,
                              int octant) {
    int i;
    for (i = offset; i <= radius; i++) {
        point_t cur_cell = FOV_get_starting_cell(origin, i, octant);
        slope_pair_t next_slopes;
        double cur_slope =
            FOV_slope(origin->x, origin->y, cur_cell.x, cur_cell.y, octant);
        bool blocked = true;

        for (; FOV_in_arch(cur_slope, slopes, octant);
             FOV_get_next_cell(&cur_cell, octant),
             cur_slope = FOV_slope(origin->x, origin->y, cur_cell.x, cur_cell.y,
                                   octant)) {
            terrain_t t = d->map[cur_cell.y][cur_cell.x];

            if (cur_slope > slopes.start_slope) {
                continue;
            }

            d->map_observed[cur_cell.y][cur_cell.x] = 1;

            if (blocked) {
                if (!IS_OPAQUE(t)) {
                    blocked = false;
                    point_t temp = FOV_get_prev_cell(cur_cell, octant);
                    next_slopes.start_slope =
                        FOV_slope(origin->x, origin->y, temp.x, temp.y, octant);
                    next_slopes.end_slope = FOV_slope(
                        origin->x, origin->y, cur_cell.x, cur_cell.y, octant);
                }
            }

            if (!blocked) {
                if (IS_OPAQUE(t)) {
                    FOV_recursive_shadowcast(d, origin, next_slopes, offset + 1,
                                             radius - 1, octant);
                    blocked = true;
                } else if (cur_slope <= slopes.end_slope) {
                    next_slopes.end_slope = FOV_slope(
                        origin->x, origin->y, cur_cell.x, cur_cell.y, octant);
                    FOV_recursive_shadowcast(d, origin, next_slopes, offset + 1,
                                             radius - 1, octant);
                    blocked = true;
                } else {
                    next_slopes.end_slope = FOV_slope(
                        origin->x, origin->y, cur_cell.x, cur_cell.y, octant);
                }
            }

            if (cur_slope <= slopes.end_slope &&
                IS_OPAQUE(t)) {
                break;
            } else if (cur_slope <= slopes.end_slope &&
                       IS_OPAQUE(t)) {
                return;
            }
            /*
                        if (blocked) {
                            if (!IS_OPAQUE(t)) {
                                blocked = false;
                                start = cur_cell;
                                end = cur_cell;
                            } else {
                                continue;
                            }
                        } else {
                            if (IS_OPAQUE(t)) {
                                blocked = true;
                                end = cur_cell;

                                FOV_recursive_shadowcast(d, origin,
                                                         FOV_slope(origin,
               &start, octant), FOV_slope(origin, &end, octant), offset + 1,
               radius - 1, octant);

                            } else {
                                continue;
                            }
                        }
                        */
        }
    }
}

void FOV_clear(dungeon_t *d) {
    int i, j;
    for (i = 0; i < DUNGEON_Y; i++) {
        for (j = 0; j < DUNGEON_X; j++) {
            d->map_observed[i][j] = 0;
        }
    }
}
