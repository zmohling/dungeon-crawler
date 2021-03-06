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

#ifndef DUNGEON_H
#define DUNGEON_H

#include <stdbool.h>
#include <stdint.h>
#include <vector>

#include "character.h"
#include "event.h"
#include "geometry.h"
#include "heap.h"
#include "object.h"

#define DUNGEON_X 80
#define DUNGEON_Y 21
#define MIN_ROOMS 6
#define MAX_ROOMS 8
#define ROOM_MIN_X 4
#define ROOM_MIN_Y 3
#define ROOM_MAX_X 12
#define ROOM_MAX_Y 8
#define STAIRS_MAX 4
#define PC_SPEED 10
#define NPC_MAX_SPEED 15
#define MONSTERS_MAX 30
#define OBJECTS_MAX 10

typedef enum __attribute__((__packed__)) terrain_type {
  ter_empty,
  ter_rock,
  ter_rock_immutable,
  ter_wall_vertical,
  ter_wall_horizontal,
  ter_floor,
  ter_floor_room,
  ter_floor_hall,
  ter_stairs,
  ter_stairs_up,
  ter_stairs_down
} terrain_t;

#define IS_SOLID(terrain)                                           \
  ((terrain == ter_rock || terrain == ter_rock_immutable ||         \
    terrain == ter_wall_vertical || terrain == ter_wall_horizontal) \
       ? 1                                                          \
       : 0)
class character_t;
class event_t;
class monster_description;
class object_description;

class corridor_path_t {
  heap_node_t *hn;
  uint8_t pos[2];
  uint8_t from[2];
  int32_t cost;
};

class room_t {
 public:
  point_t coordinates;
  uint8_t width, height;
};

class dungeon_t {
 public:
  character_t *pc;
  uint8_t num_monsters;
  uint8_t num_objects;

  heap_t event_queue;
  event_t *events;
  character_t *character_map[DUNGEON_Y][DUNGEON_X];
  character_t *characters;
  object *objects;

  uint8_t non_tunnel_distance_map[DUNGEON_Y][DUNGEON_X];
  uint8_t tunnel_distance_map[DUNGEON_Y][DUNGEON_X];
  terrain_t map[DUNGEON_Y][DUNGEON_X];
  terrain_t map_observed[DUNGEON_Y][DUNGEON_X];
  bool map_visible[DUNGEON_Y][DUNGEON_X];
  uint8_t hardness_map[DUNGEON_Y][DUNGEON_X];
  object *object_map[DUNGEON_Y][DUNGEON_X];

  uint16_t num_rooms;
  room_t *rooms;
  uint16_t num_stairs_up;
  point_t *stairs_up;
  uint16_t num_stairs_down;
  point_t *stairs_down;

  std::vector<monster_description> monster_descriptions;
  std::vector<object_description> object_descriptions;
};

void render_dungeon(dungeon_t *);
void render_room_borders(dungeon_t *);
void render_hardness_map(dungeon_t *);
void render_movement_cost_map(dungeon_t *);
void render_distance_map(dungeon_t *);
void render_tunnel_distance_map(dungeon_t *);

int init_terrain(dungeon_t *);
int deep_free_dungeon(dungeon_t *);

int read_dungeon_from_disk(dungeon_t *, char *);
int write_dungeon_to_disk(dungeon_t *, char *);

int generate_dungeon(dungeon_t *);
int generate_border(dungeon_t *);
int generate_rooms(dungeon_t *);
int generate_corridors(dungeon_t *);
int generate_staircases(dungeon_t *);
int generate_hardness(dungeon_t *);
int generate_terrain(dungeon_t *);

bool intersects(room_t *, room_t *, int);
bool out_of_bounds(room_t *, int, int);
int has_neighbor(dungeon_t *, terrain_t, point_t);
room_t *get_room(dungeon_t *, point_t *, int);
point_t get_valid_point(dungeon_t *, bool);
point_t get_valid_point_object(dungeon_t *);

#endif
