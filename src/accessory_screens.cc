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

#include "accessory_screens.h"

#include <math.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "character.h"
#include "dungeon.h"
#include "dungeon_crawler.h"
#include "fov.h"
#include "screens/window.h"

static dungeon_t *d_static;

/* This function is for sorting the characters array
 * in the dungeon struct by distance from the PC. This
 * is so the Monster List displays the closest monsters
 * at the top.
 */
static int quick_sort_compare(const void *key, const void *with) {
  int key_distance, with_distance;

  if ((*((character **)key))->is_pc()) {
    return -1;
  } else if (!(*((character **)key))->is_alive) {
    return 1;
  } else if ((*((npc **)key))->characteristics & 0x04) {
    key_distance = d_static->tunnel_distance_map[(*((npc **)key))->position.y]
                                                [(*((npc **)key))->position.x];
  } else {
    key_distance =
        d_static->non_tunnel_distance_map[(*((npc **)key))->position.y]
                                         [(*((npc **)key))->position.x];
  }

  if ((*((character **)with))->is_pc()) {
    return 1;
  } else if (!(*((character **)with))->is_alive) {
    return -1;
  } else if ((*((npc **)with))->characteristics & 0x04) {
    with_distance =
        d_static->tunnel_distance_map[(*((npc **)with))->position.y]
                                     [(*((npc **)with))->position.x];
  } else {
    with_distance =
        d_static->non_tunnel_distance_map[(*((npc **)with))->position.y]
                                         [(*((npc **)with))->position.x];
  }

  return key_distance - with_distance;
}

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

static void get_mag_and_direction(dungeon_t *d, character *c, int *m1,
                                  char **dir1, int *m2, char **dir2) {
  int y = d->pc->position.y - c->position.y;
  *m1 = abs(y);
  if (y > 0) {
    *dir1 = (char *)malloc(strlen("North") + 1);
    strcpy(*dir1, "North");
  } else {
    *dir1 = (char *)malloc(strlen("South") + 1);
    strcpy(*dir1, "South");
  }

  int x = d->pc->position.x - c->position.x;
  *m2 = abs(x);
  if (x > 0) {
    *dir2 = (char *)malloc(strlen("West") + 1);
    strcpy(*dir2, "West");
  } else {
    *dir2 = (char *)malloc(strlen("East") + 1);
    strcpy(*dir2, "East");
  }
}

static int print_monsters(dungeon_t *d, character *sorted_character_arr[],
                          int index, int height, int width, int starty,
                          int startx) {
  int inner_bound_y = starty + 2, inner_bound_x = startx + 1, i;
  int lines = (height - 4);
  int num_monsters_alive = 0;

  /* Get number of alive monsters */
  for (i = 1; i < d->num_monsters + 1; i++) {
    if (sorted_character_arr[i]->is_alive) {
      num_monsters_alive++;
    }
  }

  int adjusted_index = index;
  if (index < 0 || num_monsters_alive <= lines) {
    adjusted_index = 0;
  } else if (index > (num_monsters_alive - (lines))) {
    adjusted_index = (num_monsters_alive - (lines));
  }

  int longitudinal_magnitude, lateral_magnitude;
  char *longitudinal_card_dir, *lateral_card_dir;

  for (i = 0; i < lines && i < num_monsters_alive; i++) {
    /* Print if monster is alive and visible to PC */
    character *monster = sorted_character_arr[adjusted_index + i + 1];
    bool monster_is_visible =
        d->map_visible[monster->position.y][monster->position.x] ||
        FOV_get_fog() == false;
    if (monster->is_alive && monster_is_visible) {
      get_mag_and_direction(d, sorted_character_arr[adjusted_index + i + 1],
                            &longitudinal_magnitude, &longitudinal_card_dir,
                            &lateral_magnitude, &lateral_card_dir);

      mvprintw(inner_bound_y + i, inner_bound_x, "  %c: %2d %s and %2d %s",
               sorted_character_arr[adjusted_index + i + 1]->symbol,
               longitudinal_magnitude, longitudinal_card_dir, lateral_magnitude,
               lateral_card_dir);
      free(longitudinal_card_dir);
      free(lateral_card_dir);
    } else if (monster->is_alive && !monster_is_visible) {
      mvprintw(inner_bound_y + i, inner_bound_x, "  *: ** ***** *** ** ****");
    }
  }

  return adjusted_index;
}

void monster_list(dungeon_t *d) {
  d_static = d;
  int ch, height = 12, width = 29;
  int starty = ((DUNGEON_Y - height) / 2) + 1, startx = (DUNGEON_X - width) / 2;
  WINDOW *w = create_newwin(height, width, starty, startx);
  overwrite(w, stdscr);

  char *title = (char *)"MONSTER LIST";
  char *footer = (char *)"PRESS ESC TO EXIT";
  mvprintw(starty, startx + (width / 2) - (strlen(title) / 2), title);
  mvprintw(starty + height - 1, startx + (width / 2) - (strlen(footer) / 2),
           footer);

  /* Copy of Dungeon's characters array. Characters and Character map
   * became out of sync when sorting the array itself. */
  character *sorted_character_arr[d->num_monsters + 1];

  int i;
  for (i = 0; i < d->num_monsters + 1; i++) {
    sorted_character_arr[i] = &(d->characters[i]);
  }

  qsort(sorted_character_arr, d->num_monsters + 1, sizeof(character *),
        &quick_sort_compare);

  int index = 0;
  print_monsters(d, sorted_character_arr, index, height, width, starty, startx);
  while ((ch = getch()) != 27) {
    switch (ch) {
      case 'k':
      case KEY_UP:
        index = print_monsters(d, sorted_character_arr, --index, height, width,
                               starty, startx);
        break;
      case 'j':
      case KEY_DOWN:
        index = print_monsters(d, sorted_character_arr, ++index, height, width,
                               starty, startx);
        break;
    }

    window_close(&w);
    // delwin(w);
  }
}

/* Win and Lost endscreen */
void endscreen(int didWin) {
  WINDOW *local_win;

  local_win = newwin(24, 80, 0, 0);
  box(local_win, 0, 0);
  wrefresh(local_win);

  char variableStr[81];
  if (didWin) {
    strncpy(variableStr, "<   You Won!   >", 81);
  } else {
    strncpy(variableStr, "< You've Died! >", 81);
  }

  mvprintw(6, 25, " ______________ ");
  mvprintw(7, 25, "%s", variableStr);
  mvprintw(8, 25, " -------------- ");
  mvprintw(9, 25, "        \\   ^__^");
  mvprintw(10, 25, "         \\  (oo)\\_______");
  mvprintw(11, 25, "            (__)\\       )\\/\\");
  mvprintw(12, 25, "                ||----w |");
  mvprintw(13, 25, "                ||     ||");
  mvprintw(16, 30, "PRESS 'q' TO QUIT");
  refresh();

  while (getch() != 'q')
    ;

  quit();
}
