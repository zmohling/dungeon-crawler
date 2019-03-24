#include "fov.h"

#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "dungeon.h"
#include "geometry.h"

static double FOV_slope(point_t *p1, point_t *p2, int octant) {
    switch (octant) {
        case 1:
        case 4:
        case 5:
        case 8:
            return (fabs((p1->y - 0.5) - (p2->y - 0.5)) /
                    fabs((p1->x - 0.5) - (p2->x - 0.5)));
        case 2:
        case 3:
        case 6:
        case 7:
            return 1 / (fabs((p1->y - 0.5) - (p2->y - 0.5)) /
                        fabs((p1->x - 0.5) - (p2->x - 0.5)));
    }

    exit(-8);
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
        case 4:
        case 3:
            p.x -= units_from_src;
            p.y -= units_from_src;
            break;
        case 6:
        case 7:
            p.x -= units_from_src;
            p.y += units_from_src;
            break;
        case 5:
        case 8:
            p.x += units_from_src;
            p.y += units_from_src;
            break;
    }

    return p;
}

static int FOV_in_arch(point_t *src, point_t *dest, double start_slope,
                       double end_slope, int octant) {
    double slope = (double)roundf(FOV_slope(src, dest, octant) * 100) / 100.0;

    return (slope <= 1.00f && slope >= 0);
}

void FOV_recursive_shadowcast(dungeon_t *d, double start_slope,
                              double end_slope, int y, int x, int octant,
                              int radius) {
    int i;
    for (i = 1; i <= radius; i++) {
        point_t cur_cell = FOV_get_starting_cell(&d->pc->position, i, octant);
        point_t *start = NULL, *end = NULL;

        for (; FOV_in_arch(&d->pc->position, &cur_cell, start_slope, end_slope,
                           octant);
             FOV_get_next_cell(&cur_cell, octant)) {
            terrain_t t = d->map[cur_cell.y][cur_cell.x];
            if (FOV_slope(&d->pc->position, &cur_cell, octant) > start_slope) {
                continue;
            } else if (IS_OPAQUE(t) && start == NULL) {
                continue;
            } else if (!IS_OPAQUE(t) && start == NULL) {
                start = malloc(sizeof(point_t));
                start->x = cur_cell.x;
                start->y = cur_cell.y;

                continue;
            } else if (!IS_OPAQUE(t) && start != NULL) {
                continue;
            } else if (IS_OPAQUE(t) && start != NULL && end == NULL) {
                end = malloc(sizeof(point_t));
                end->x = cur_cell.x;
                end->y = cur_cell.y;
            }

            d->map_observed[cur_cell.y][cur_cell.x] = 1;

            FOV_recursive_shadowcast(d,
                                     FOV_slope(&d->pc->position, start, octant),
                                     FOV_slope(&d->pc->position, end, octant),
                                     y, x, octant, radius - 1);
            free(start); free(end);
        }
    }
}

