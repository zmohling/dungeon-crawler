#include <ncurses.h>
#include <stdlib.h>

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

    /*Clear status line */
    move(0, 0);
    clrtoeol();

    do {
        key_is_valid = 1;
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
                break;
            case '<':
                use_stairs(d, pc_up_stairs);
                break;
            case 'm':
                break;
            case KEY_UP:
                break;
            case KEY_DOWN:
                break;
            case 27:
                break;
            case 'q':
                quit();
                //d->pc->is_alive = false;
                break;
            default:
                mvprintw(0, 0, "%s", "Invalid Input");
                get_new_input(&key_is_valid, &input);
                break;
        }

    } while (!key_is_valid);
}

