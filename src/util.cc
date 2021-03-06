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

#include "util.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

/*
 * Iterates through *argv for specified
 * switch *s. Returns true if *argv contains
 * the string *s.
 */
bool contains(int argc, char *argv[], char *s, int *n) {
    int i;
    for (i = 0; i < argc; i++) {
        if (!(strcmp(argv[i], s))) {
            *n = i;
            return true;
        }
    }

    return false;
}

/*
 * Initialize path to ~/.rlg327/ and
 * make it if not already existent. Returns
 * complete path the dungeon file.
 */
int path_init(char **path, char *filename) {
    /* FS setup */
    char *home = getenv("HOME");
    char *dir = (char *) malloc(strlen(home) + strlen("/.rlg327/") + 1);
    strcpy(dir, home);
    strcat(dir, "/.rlg327/");

    if (mkdir(dir, 0755)) {
        /* errno == 17 is an existing directory */
        if (errno != 17) {
            fprintf(stderr,
                    "Error: ~/.rlg327/ could not be found nor created. (%d)\n",
                    errno);
            exit(-1);
        }
    }

    /* Dot file directory's path */
    *path = (char *) malloc(strlen(home) + strlen("/.rlg327/") + strlen(filename) + 1);
    //*path = malloc(strlen("bin/saved_dungeons/03.rlg327") + 1);
    strcpy(*path, home);
    strcat(*path, "/.rlg327/");
    strcat(*path, filename);
    //strcat(*path, "bin/saved_dungeons/03.rlg327");

    free(dir);

    return 0;
}

