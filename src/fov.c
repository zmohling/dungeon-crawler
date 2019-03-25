#include "fov.h"

#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "dungeon.h"
#include "geometry.h"

static void FOV_slopes_from_src(point_t src, point_t dest,
                                slope_pair_t *cell_slopes, int octant) {
    switch (octant) {
        case 1:
            cell_slopes->start_slope =
                -1.0 * (((dest.y - 1.0) - (src.y - 0.5)) /
                        ((dest.x - 1.0) - (src.x - 0.5)));
            cell_slopes->end_slope = -1.0 * (((dest.y) - (src.y - 0.5)) /
                                             ((dest.x) - (src.x - 0.5)));
            break;
        case 5:
            cell_slopes->start_slope = -1.0 * (((dest.y) - (src.y - 0.5)) /
                                               ((dest.x) - (src.x - 0.5)));
            cell_slopes->end_slope = -1.0 * (((dest.y - 1.0) - (src.y - 0.5)) /
                                             ((dest.x - 1.0) - (src.x - 0.5)));
            break;
        case 4:
            cell_slopes->start_slope = 1.0 * (((dest.y - 1.0) - (src.y - 0.5)) /
                                              ((dest.x) - (src.x - 0.5)));
            cell_slopes->end_slope = 1.0 * (((dest.y) - (src.y - 0.5)) /
                                            ((dest.x - 1.0) - (src.x - 0.5)));
            break;
        case 8:
            cell_slopes->start_slope = 1.0 * (((dest.y) - (src.y - 0.5)) /
                                              ((dest.x - 1.0) - (src.x - 0.5)));
            cell_slopes->end_slope = 1.0 * (((dest.y - 1.0) - (src.y - 0.5)) /
                                            ((dest.x) - (src.x - 0.5)));
            break;
        case 3:
            cell_slopes->start_slope = 1.0 / (((dest.y) - (src.y - 0.5)) /
                                              ((dest.x - 1.0) - (src.x - 0.5)));
            cell_slopes->end_slope = 1.0 / (((dest.y - 1.0) - (src.y - 0.5)) /
                                            ((dest.x) - (src.x - 0.5)));
            break;
        case 7:
            cell_slopes->start_slope = 1.0 / (((dest.y - 1.0) - (src.y - 0.5)) /
                                              ((dest.x) - (src.x - 0.5)));
            cell_slopes->end_slope = 1.0 / (((dest.y) - (src.y - 0.5)) /
                                            ((dest.x - 1.0) - (src.x - 0.5)));
            break;
        case 2:
            cell_slopes->start_slope = -1.0 / (((dest.y) - (src.y - 0.5)) /
                                               ((dest.x) - (src.x - 0.5)));
            cell_slopes->end_slope = -1.0 / (((dest.y - 1.0) - (src.y - 0.5)) /
                                             ((dest.x - 1.0) - (src.x - 0.5)));
            break;
        case 6:
            cell_slopes->start_slope =
                -1.0 / (((dest.y - 1.0) - (src.y - 0.5)) /
                        ((dest.x - 1.0) - (src.x - 0.5)));
            cell_slopes->end_slope = -1.0 / (((dest.y) - (src.y - 0.5)) /
                                             ((dest.x) - (src.x - 0.5)));
            break;
    }

    cell_slopes->start_slope =
        ((double)roundf(cell_slopes->start_slope * 100) / 100);

    cell_slopes->end_slope =
        ((double)roundf(cell_slopes->end_slope * 100) / 100);
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

static void FOV_normalize_slopes(slope_pair_t *slopes) {
    if (slopes->start_slope > 1.0) slopes->start_slope = 1.0;
    if (slopes->end_slope < 0.0) slopes->end_slope = 0.0;
}

void FOV_recursive_shadowcast(dungeon_t *d, point_t *origin,
                              slope_pair_t slopes, int offset, int radius,
                              int octant) {
    if (slopes.start_slope < slopes.end_slope) {
        return;
    }
    int i;
    for (i = offset; i <= radius; i++) {
        point_t cur_cell = FOV_get_starting_cell(origin, i, octant);
        slope_pair_t cell_slopes;
        bool blocked = true;

        for (int j = 0; j <= i; j++) {
            FOV_slopes_from_src(*origin, cur_cell, &cell_slopes, octant);

            if (cell_slopes.end_slope > slopes.start_slope) {
                FOV_get_next_cell(&cur_cell, octant);
                continue;
            } else if (cell_slopes.start_slope <= slopes.end_slope) {
                break;
            }

            d->map_observed[cur_cell.y][cur_cell.x] = 1;
            terrain_t t = d->map[cur_cell.y][cur_cell.x];

            if (blocked) {
                if (!IS_OPAQUE(t)) {
                    blocked = false;
                } else {
                    slopes.start_slope = cell_slopes.end_slope;
                    FOV_normalize_slopes(&slopes);
                }
            }

            if (!blocked) {
                if (IS_OPAQUE(t)) {
                    slope_pair_t temp = slopes;
                    temp.end_slope = cell_slopes.start_slope;
                    FOV_normalize_slopes(&temp);

                    FOV_recursive_shadowcast(d, origin, temp, offset + 1,
                                             radius - 1, octant);
                    blocked = true;

                    continue;
                } else if (cell_slopes.start_slope >= slopes.end_slope &&
                           cell_slopes.end_slope <= slopes.end_slope) {
                    if (IS_OPAQUE(t)) {
                        return;
                    } else {
                        //slopes.end_slope = cell_slopes.end_slope;
                        //FOV_normalize_slopes(&slopes);
                        break;
                    }
                } else {
                    //slopes.end_slope = cell_slopes.end_slope;
                    //FOV_normalize_slopes(&slopes);
                }
            }

            FOV_get_next_cell(&cur_cell, octant);
        }
    }
}

void FOV_shadowcast(dungeon_t *d, point_t *origin, int radius) {
    FOV_clear(d);
    d->map_observed[origin->y][origin->x] = 1;

    slope_pair_t init_slopes;
    init_slopes.start_slope = 1.0;
    init_slopes.end_slope = 0.0;

    int i;
    for (i = 1; i <= 8; i++) {
        FOV_recursive_shadowcast(d, origin, init_slopes, 1, radius, i);
    }
}

void FOV_clear(dungeon_t *d) {
    int i, j;
    for (i = 0; i < DUNGEON_Y; i++) {
        for (j = 0; j < DUNGEON_X; j++) {
            if (d->map_observed[i][j] == 1) {
                d->map_observed[i][j] = 2;
            }
        }
    }
}
