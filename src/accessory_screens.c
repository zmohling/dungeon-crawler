#include <math.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "accessory_screens.h"
#include "character.h"
#include "dungeon.h"

static WINDOW *create_newwin(int height, int width, int starty, int startx) {
    WINDOW *local_win;

    local_win = newwin(height, width, starty, startx);
    werase(local_win);
    box(local_win, 0, 0); /* 0, 0 gives default characters
                           * for the vertical and horizontal
                           * lines			*/
    wrefresh(local_win);  /* Show that box 		*/

    return local_win;
}

static void get_mag_and_direction(dungeon_t *d, character_t *c, int *m1,
                                  char **dir1, int *m2, char **dir2) {
    int y = d->pc->position.y - c->position.y;
    *m1 = abs(y);
    if (y < 0) {
        *dir1 = malloc(strlen("North") + 1);
        strcpy(*dir1, "North");
    } else {
        *dir1 = malloc(strlen("South") + 1);
        strcpy(*dir1, "South");
    }

    int x = d->pc->position.x - c->position.x;
    *m2 = abs(x);
    if (x < 0) {
        *dir2 = malloc(strlen("West") + 1);
        strcpy(*dir2, "West");
    } else {
        *dir2 = malloc(strlen("East") + 1);
        strcpy(*dir2, "East");
    }
}

static int print_monsters(dungeon_t *d, int index, int height, int width,
                          int starty, int startx) {
    int inner_bound_y = starty + 2, inner_bound_x = startx + 1, i;
    int lines = (inner_bound_y + (height - 4));

    int adjusted_index = (d->num_monsters - lines - index > 0) ? index : 0;
    if (adjusted_index < 0) {
        adjusted_index = 0;
    } else if (adjusted_index <= lines) {
    } else if (adjusted_index > d->num_monsters - lines - index) {
        adjusted_index = d->num_monsters - lines - index;
    }

    int longitudinal_magnitude, lateral_magnitude;
    char *longitudinal_card_dir, *lateral_card_dir;

    int c = 0;
    for (i = inner_bound_y;
         i < (inner_bound_y + (height - 4)) && c < d->num_monsters; i++, c++) {
        if (d->characters[adjusted_index + c + 1].is_alive) {
            get_mag_and_direction(d, &d->characters[adjusted_index + c + 1],
                                  &longitudinal_magnitude,
                                  &longitudinal_card_dir, &lateral_magnitude,
                                  &lateral_card_dir);

            mvprintw(i, inner_bound_x, "  %x\t%d %s and %d %s",
                     d->characters[adjusted_index + c + 1].symbol & 0xff,
                     longitudinal_magnitude, longitudinal_card_dir,
                     lateral_magnitude, lateral_card_dir);

            free(longitudinal_card_dir);
            free(lateral_card_dir);
        }
    }

    return adjusted_index;
}

/* TODO: Nice confirmation menu*/
void quit() {
    endwin();
    exit(0);
}

void monster_list(dungeon_t *d) {
    int ch, height = 17, width = 30;
    int starty = ((DUNGEON_Y - height) / 2) + 1,
        startx = (DUNGEON_X - width) / 2;
    WINDOW *w = create_newwin(height, width, starty, startx);
    overwrite(w, stdscr);

    char *title = "MONSTER LIST";
    char *footer = "PRESS ESC TO EXIT";
    mvprintw(starty + 1, startx + (width / 2) - (strlen(title) / 2), title);
    mvprintw(starty + height - 2, startx + (width / 2) - (strlen(footer) / 2),
             footer);

    int index = 0;
    print_monsters(d, index, height, width, starty, startx);
    while ((ch = getch()) != 27) {
        switch (ch) {
            case 'k':
            case KEY_UP:
                index =
                    print_monsters(d, --index, height, width, starty, startx);
                break;
            case 'j':
            case KEY_DOWN:
                index =
                    print_monsters(d, ++index, height, width, starty, startx);
                break;
        }

        delwin(w);
    }
}

