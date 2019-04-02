/*
 *  Copyright (C) 2019 Zachary Mohling
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "fov.h"

#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "dungeon.h"
#include "geometry.h"

static dungeon_t *d_static;
static bool FOV_fog_enabled = true;

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
    int x = src->x;
    int y = src->y;

    switch (octant) {
        case 1:
        case 2:
            x += units_from_src;
            y -= units_from_src;
            break;
        case 3:
        case 4:
            x -= units_from_src;
            y -= units_from_src;
            break;
        case 5:
        case 6:
            x -= units_from_src;
            y += units_from_src;
            break;
        case 7:
        case 8:
            x += units_from_src;
            y += units_from_src;
            break;
    }

    if (x < 0) x = 0;
    if (x > (DUNGEON_X - 1)) x = (DUNGEON_X - 1);
    if (y < 0) y = 0;
    if (y > (DUNGEON_Y - 1)) y = (DUNGEON_Y - 1);

    p.x = x;
    p.y = y;

    return p;
}

static double FOV_euclidean_distance(point_t p1, point_t p2) {
    return sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2));
}

static int FOV_does_pass_light(dungeon_t *d, point_t *origin,
                               point_t *cur_cell) {
    double distance = FOV_euclidean_distance(*origin, *cur_cell);
    terrain_t cur_t = d->map[cur_cell->y][cur_cell->x];

    d->map_observed[cur_cell->y][cur_cell->x] = cur_t;
    d->map_visible[cur_cell->y][cur_cell->x] = true;

    if (!IS_SOLID(cur_t) &&
        (islessequal(distance, 1.0) || get_room(d, cur_cell, 0.0))) {
        return true;
    } else {
        return false;
    }
}

static void FOV_normalize_slopes(slope_pair_t *slopes) {
    if (slopes->start_slope > 1.0) slopes->start_slope = 1.0;
    if (slopes->end_slope < 0.0) slopes->end_slope = 0.0;
}

void FOV_recursive_shadowcast(dungeon_t *d, point_t *origin,
                              slope_pair_t slopes, int offset, int radius,
                              int octant) {
    int i;
    for (i = offset; i <= radius; i++) {
        point_t cur_cell = FOV_get_starting_cell(origin, i, octant);
        slope_pair_t cell_slopes;
        bool blocked = true;

        for (int j = 0; j <= i; j++) {
            FOV_slopes_from_src(*origin, cur_cell, &cell_slopes, octant);

            if (slopes.start_slope < slopes.end_slope) {
                return;
            } else if (isgreater(cell_slopes.end_slope, slopes.start_slope)) {
                FOV_get_next_cell(&cur_cell, octant);
                continue;
            } else if (isless(cell_slopes.start_slope, slopes.end_slope)) {
                break;
            }

            int transparent = FOV_does_pass_light(d, origin, &cur_cell);
            if (blocked) {
                if (transparent) {
                    blocked = false;
                } else {
                    slopes.start_slope = cell_slopes.end_slope;
                    FOV_normalize_slopes(&slopes);
                }
            }

            if (!blocked) {
                if (!transparent) {
                    slope_pair_t temp = slopes;
                    temp.end_slope = cell_slopes.start_slope;
                    FOV_normalize_slopes(&temp);

                    FOV_recursive_shadowcast(d, origin, temp, offset + 1,
                                             radius - 1, octant);
                    blocked = true;
                    slopes.start_slope = cell_slopes.end_slope;
                    FOV_normalize_slopes(&slopes);

                } else if (isgreaterequal(cell_slopes.start_slope,
                                          slopes.end_slope) &&
                           islessequal(cell_slopes.end_slope,
                                       slopes.end_slope)) {
                    if (!transparent) {
                        return;
                    } else {
                        break;
                    }
                }
            }
            FOV_get_next_cell(&cur_cell, octant);
        }
    }
}

void FOV_shadowcast(dungeon_t *d, point_t *origin, int radius) {
    d_static = d;

    FOV_clear(d);

    d->map_observed[origin->y][origin->x] = d->map[origin->y][origin->x];
    d->map_visible[origin->y][origin->x] = true;

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
            if (d->map_visible[i][j]) {
                d->map_visible[i][j] = false;
            }
        }
    }
}

bool FOV_get_fog() { return FOV_fog_enabled; }

void FOV_toggle_fog() {
    FOV_fog_enabled = !FOV_fog_enabled;
    render_dungeon(d_static);
}
