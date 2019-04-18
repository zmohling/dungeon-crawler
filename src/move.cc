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

#include "move.h"

#include <limits.h>
#include <math.h>
#include <ncurses.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "character.h"
#include "dungeon.h"
#include "geometry.h"
#include "input.h"
#include "path.h"

static void move_helper(dungeon_t *, character *, point_t *);

/* Handles PC movement. Moves and returns 0 if a valid movement,
 * or displays a message and returns a nonzero value if that
 * move connot be done.
 */
int move_pc(dungeon_t *d, point_t *p, int ch) {
  character *pc = d->pc;

  point_t next_pos;
  next_pos.x = pc->position.x;
  next_pos.y = pc->position.y;

  if (p != NULL) {
    next_pos.x = p->x;
    next_pos.y = p->y;
  } else {
    /* Stage next_pos per input char */
    switch (ch) {
      case '7':
      case 'y':
        next_pos.x = next_pos.x - 1;
        next_pos.y = next_pos.y - 1;
        break;
      case '8':
      case 'k':
        next_pos.x = next_pos.x;
        next_pos.y = next_pos.y - 1;
        break;
      case '9':
      case 'u':
        next_pos.x = next_pos.x + 1;
        next_pos.y = next_pos.y - 1;
        break;
      case '6':
      case 'l':
        next_pos.x = next_pos.x + 1;
        next_pos.y = next_pos.y;
        break;
      case '3':
      case 'n':
        next_pos.x = next_pos.x + 1;
        next_pos.y = next_pos.y + 1;
        break;
      case '2':
      case 'j':
        next_pos.x = next_pos.x;
        next_pos.y = next_pos.y + 1;
        break;
      case '1':
      case 'b':
        next_pos.x = next_pos.x - 1;
        next_pos.y = next_pos.y + 1;
        break;
      case '4':
      case 'h':
        next_pos.x = next_pos.x - 1;
        next_pos.y = next_pos.y;
        break;
      case '5':
      case '.':
      case ' ':
        return 0;
      default:
        next_pos.x = 0;
        next_pos.y = 0;
        break;
    }
  }
  /* Check if next_pos is valid and trample monsters */
  if (!IS_SOLID(d->map[next_pos.y][next_pos.x])) {
    if (!(check_for_trample(d, pc, next_pos.x, next_pos.y))) {
      mvprintw(0, 0, "You slayed a monster!");
    }

    move_helper(d, pc, &next_pos);

    /* Update Distance Maps */
    non_tunnel_distance_map(d);
    tunnel_distance_map(d);

    return 0;
  } else {
    return 1;
  }
}

/* This function generates a new dungeon when
 * the PC goes up or down stairs. Places the PC
 * on an appropriate staircase.
 */
int use_stairs(dungeon_t *d, pc_movement_t p) {
  deep_free_dungeon(d);
  generate_dungeon(d);

  d->pc = reinterpret_cast<character *>(malloc(sizeof(character)));
  if (p == pc_up_stairs) {
    d->pc->position.x = d->stairs_down[0].x;
    d->pc->position.y = d->stairs_down[0].y;
  } else if (p == pc_down_stairs) {
    d->pc->position.x = d->stairs_up[0].x;
    d->pc->position.y = d->stairs_up[0].y;
  }
  d->num_monsters = (rand() % 5) + 3;

  event_simulator_start(d);
  return 0;
}

/* Jump table for NPC move functions.
 * TODO: Implement jtable and more move functions
 */
int move_npc(dungeon_t *d, npc *c) {
  if (c->characteristics & 0x08) {
    if (rand() % 2) {
      move_npc_erratic(d, c);
      return 0;
    }
  }

  if (c->characteristics & 0x04) {
    move_npc_tunnel(d, c);
  } else {
    if (d->non_tunnel_distance_map[c->position.y][c->position.x] < 12) {
      move_npc_non_tunnel(d, c);
    }
  }

  return 0;
}

/* Move non-tunneling NPCs by a gradient.
 * TODO: Implement a dijkstra's version for
 * smarter NPCs
 */
int move_npc_non_tunnel(dungeon_t *d, npc *c) {
  uint8_t distance = UINT8_MAX;
  point_t next_pos;
  next_pos.x = c->position.x;
  next_pos.y = c->position.y;

  int x, y;
  for (y = (c->position.y - 1); y <= (c->position.y + 1); y++) {
    for (x = (c->position.x - 1); x <= (c->position.x + 1); x++) {
      if (y > (DUNGEON_Y - 1) || y < 0 || x > (DUNGEON_X - 1) || x < 0) {
        continue;
      } else if (IS_SOLID(d->map[y][x])) {
        continue;
      }

      /* Prefers cardinal directions */
      if (d->non_tunnel_distance_map[y][x] < distance ||
          (d->non_tunnel_distance_map[y][x] <= distance &&
           ((abs(x - c->position.x) + abs(y - c->position.y)) == 1))) {
        distance = d->non_tunnel_distance_map[y][x];
        next_pos.x = x;
        next_pos.y = y;
      }
    }
  }

  check_for_trample(d, c, next_pos.x, next_pos.y);
  move_helper(d, c, &next_pos);

  return 0;
}

/* Move tunneling NPCs by a gradient.
 * TODO: Implement a dijkstra's version for
 * smarter NPCs
 */
int move_npc_tunnel(dungeon_t *d, npc *c) {
  uint8_t min_cost = UINT8_MAX;
  point_t next_pos;
  next_pos.x = c->position.x;
  next_pos.y = c->position.y;

  int x, y;
  for (y = (c->position.y - 1); y <= (c->position.y + 1); y++) {
    for (x = (c->position.x - 1); x <= (c->position.x + 1); x++) {
      if (y > (DUNGEON_Y - 1) || y < 0 || x > (DUNGEON_X - 1) || x < 0) {
        continue;
      } else if (d->map[y][x] == ter_rock_immutable) {
        continue;
      }

      uint8_t cost =
          d->tunnel_distance_map[y][x] + (d->hardness_map[y][x] / 85);

      /* Prefers cardinal directions */
      if (cost < min_cost ||
          (cost <= min_cost &&
           ((abs(x - c->position.x) + abs(y - c->position.y)) == 1))) {
        min_cost = cost;
        next_pos.x = x;
        next_pos.y = y;
      }
    }
  }

  /* Remove hardness if next_pos is of wall terrain*/
  if (d->hardness_map[next_pos.y][next_pos.x] > 85) {
    d->hardness_map[next_pos.y][next_pos.x] -= 85;
    tunnel_distance_map(d);

    return 0;
  } else if (d->hardness_map[next_pos.y][next_pos.x] <= 85 &&
             d->hardness_map[next_pos.y][next_pos.x] > 0) {
    d->hardness_map[next_pos.y][next_pos.x] = 0;
    d->map[next_pos.y][next_pos.x] = ter_floor_hall;

    non_tunnel_distance_map(d);
    tunnel_distance_map(d);
  }

  /* Move NPC when next_pos is of floor terrain */
  if (d->hardness_map[next_pos.y][next_pos.x] == 0) {
    check_for_trample(d, c, next_pos.x, next_pos.y);
    move_helper(d, c, &next_pos);
  }

  return 0;
}

int move_npc_erratic(dungeon_t *d, npc *c) {
  point_t next_pos;
  next_pos.x = c->position.x + (rand() % 3) - 1;
  next_pos.y = c->position.y + (rand() % 3) - 1;

  if (d->hardness_map[next_pos.y][next_pos.x] == 0) {
    check_for_trample(d, c, next_pos.x, next_pos.y);
    move_helper(d, c, &next_pos);
  }

  return 0;
}

/* Checks position for intended movement. If a
 * character exists at (x, y), "kills" it and updates the
 * relevant data structures.
 */
int check_for_trample(dungeon_t *d, character *c, int x, int y) {
  if (d->character_map[y][x] != NULL && d->character_map[y][x] != c) {
    if (!(c->is_pc())) {
      npc *c = reinterpret_cast<npc *>(d->character_map[y][x]);
      // c->md->set_validity(false);
    }

    d->character_map[y][x]->is_alive = false;
    d->character_map[y][x] = NULL;

    return 0;
  }

  return 1;  // no kill
}

static void move_helper(dungeon_t *d, character *c, point_t *next_pos) {
  d->character_map[c->position.y][c->position.x] = NULL;
  c->position.x = next_pos->x;
  c->position.y = next_pos->y;
  d->character_map[c->position.y][c->position.x] = c;
}
