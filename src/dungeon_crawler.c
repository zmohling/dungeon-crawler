#include "dungeon_crawler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ncurses.h>

#include "character.h"
#include "dungeon.h"
#include "event_simulator.h"
#include "path_finder.h"
#include "util.h"

static dungeon_t dungeon;
static bool save_on_quit;
static char *path;

static void init_curses() {
    initscr();
    cbreak();
    raw();
    noecho();
    curs_set(0);
    nonl();
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE);
    set_escdelay(0);

    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_CYAN, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);

    attron(COLOR_PAIR(1));

    clear();
}

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
    path_init(&path);

    /* Dungeon*/
    memset(dungeon.character_map, 0, sizeof(character_t *) * DUNGEON_Y * DUNGEON_X);

    /* Number of monsters switch */
    char *num_monsters_str = "--nummon";
    int n = (rand() % 7) + 5;
    if (contains(argc, argv, num_monsters_str, &n)) {
        n = atoi(argv[n + 1]);

        if (n > MONSTERS_MAX) {
            dungeon.num_monsters = MONSTERS_MAX;
            fprintf(stderr, "Exceded maxium monster count of 30.\n");

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
        dungeon.pc = malloc(sizeof(character_t));
        read_dungeon_from_disk(&dungeon, path);
        generate_terrain(&dungeon);
    } else {
        generate_dungeon(&dungeon);
    }

    /* Save switch */
    char *save_str = "--save";
    if (contains(argc, argv, save_str, &n)) {
        save_on_quit = true;
    }

    /* Start Game */
    init_curses();
    event_simulator_start(&dungeon);

    return 0;
}

/* Function for ending the game. Frees memory, saves to disk, 
 * and allows ncurses to restore terminal to stashed state.
 */
void quit() {
    endwin();

    if (save_on_quit) {
        write_dungeon_to_disk(&dungeon, path);
    }

    free(path);
    deep_free_dungeon(&dungeon);

    exit(0);
}

