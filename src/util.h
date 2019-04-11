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

#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>

/* Returns true if random float in [0,1] is less than *
 * numerator/denominator.  Uses only integer math.    */
# define rand_under(numerator, denominator) \
  (rand() < ((RAND_MAX / denominator) * numerator))

/* Returns random integer in [min, max]. */
# define rand_range(min, max) ((rand() % (((max) + 1) - (min))) + (min))

#define UNUSED(f) ((void) f)
#define SAVE_DIR               ".rlg327"
#define DUNGEON_SAVE_FILE      "dungeon"
#define DUNGEON_SAVE_SEMANTIC  "RLG327-" TERM
#define DUNGEON_SAVE_VERSION   0U
#define MONSTER_DESC_FILE      "monster_desc.txt"
#define OBJECT_DESC_FILE       "object_desc.txt"

bool contains(int, char *[], char *, int *);
int path_init(char **, char *);

#endif
