#include <ncurses.h>
#include <stdlib.h>

#include "accessory_screens.h"
#include "input.h"
#include "move.h"

static void get_new_input(int *key_is_valid, int *input) {
    *key_is_valid = 0;
    *input = getch();

    return;
}

void handle_key(dungeon_t *d, int c) {
    int input = c;
    int key_is_valid;

    do {
        key_is_valid = 1;

        /*Clear status line */
        move(0, 0);
        clrtoeol();

        switch (input) {
            case '7':
            case 'y':
            case '8':
            case 'k':
            case '9':
            case 'u':
            case '6':
            case 'l':
            case '3':
            case 'n':
            case '2':
            case 'j':
            case '1':
            case 'b':
            case '4':
            case 'h':
            case '5':
            case '.':
            case ' ':
                if (move_pc(d, input)) {
                    /* Invalid */
                    mvprintw(0, 0, "%s", "You cannot move here!");
                    get_new_input(&key_is_valid, &input);
                }
                break;
            case '>':
                if (d->map[d->pc->position.y][d->pc->position.x] ==
                    ter_stairs_down) {
                    /* Valid */
                    use_stairs(d, pc_down_stairs);
                } else {
                    /* Invalid */
                    mvprintw(0, 0, "%s", "You cannot go down the stairs here!");
                    get_new_input(&key_is_valid, &input);
                }
                break;
            case '<':
                if (d->map[d->pc->position.y][d->pc->position.x] ==
                    ter_stairs_up) {
                    /* Valid */
                    use_stairs(d, pc_up_stairs);
                } else {
                    /* Invalid */
                    mvprintw(0, 0, "%s", "You cannot go up the stairs here!");
                    get_new_input(&key_is_valid, &input);
                }
                break;
            case 'm':
                monster_list(d);
                render_dungeon(d);
                get_new_input(&key_is_valid, &input);
                break;
            case KEY_UP:
                break;
            case KEY_DOWN:
                break;
            case 'q':
                quit();
                break;
            default:
                mvprintw(0, 0, "%s", "Invalid Input");
                get_new_input(&key_is_valid, &input);
                break;
        }

    } while (!key_is_valid);
}
