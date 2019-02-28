#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "character.h"
#include "dungeon.h"
#include "event_simulator.h"
#include "path_finder.h"
#include "util.h"

int main(int argc, char *argv[]) {
    /* Check arguments */
    if (argc > 5) {
        fprintf(stderr, "Ussage: %s [--save][--load][--nummon]\n", argv[0]);
        exit(-1);
    }

    /* Random seed */
    int seed = time(NULL);
    srand(seed);

    /* Initialize path to ~/.rlg327/ */
    char *path;
    path_init(&path);

    /* Dungeon*/
    dungeon_t dungeon;
    dungeon.pc = malloc(sizeof(character_t));

    /* Number of monsters switch */
    char *num_monsters_str = "--nummon";
    int n = 5;
    if (contains(argc, argv, num_monsters_str, &n)) {
        n = atoi(argv[n + 1]);

        if (n > MONSTERS_MAX) {
            dungeon.num_monsters = MONSTERS_MAX;
            fprintf(stderr, "Exceded maxium monster count of 10.\n");

            fprintf(stderr, "Ussage: %s [--save][--load][--nummon]\n", argv[0]);
            exit(1);
        } else if (n <= 0) {
            dungeon.num_monsters = 1;
            fprintf(stderr, "At least one monster is requred.\n");
            fprintf(stderr, "Ussage: %s [--save][--load][--nummon]\n", argv[0]);
            exit(1);
        } else {
            dungeon.num_monsters = n;
        }
    } else {
        dungeon.num_monsters = n;
    }

    /* Load switch */
    char *load_switch_str = "--load";
    if (contains(argc, argv, load_switch_str, &n)) {
        read_dungeon_from_disk(&dungeon, path);
        generate_terrain(&dungeon);
    } else {
        generate_dungeon(&dungeon);
    }

    /* Save switch */
    char *save_str = "--save";
    if (contains(argc, argv, save_str, &n)) {
        write_dungeon_to_disk(&dungeon, path);
    }

    /* Start Game */
    event_simulator_start(&dungeon);



    free(path);
    deep_free_dungeon(&dungeon);

    return 0;
}
