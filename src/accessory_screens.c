#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>

#include "accessory_screens.h"
#include "dungeon.h"

/* TODO: Nice confirmation menu*/
void quit() {
    endwin();
    exit(0);
}
