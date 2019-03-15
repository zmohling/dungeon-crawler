#ifndef DUNGEON_H
#define DUNGEON_H

#include <stdbool.h>
#include <stdint.h>
#include "character.h"
#include "event_simulator.h"
#include "geometry.h"
#include "heap.h"

#define DUNGEON_X 80
#define DUNGEON_Y 21
#define MIN_ROOMS 6
#define MAX_ROOMS 7
#define ROOM_MIN_X 4
#define ROOM_MIN_Y 3
#define ROOM_MAX_X 15
#define ROOM_MAX_Y 10
#define STAIRS_MAX 3
#define PC_SPEED 10
#define NPC_MAX_SPEED 15
#define MONSTERS_MAX 30

typedef enum __attribute__((__packed__)) terrain_type {
    ter_debug,
    ter_wall,
    ter_wall_immutable,
    ter_floor,
    ter_floor_room,
    ter_floor_hall,
    ter_stairs,
    ter_stairs_up,
    ter_stairs_down
} terrain_t;

typedef struct room {
    point_t coordinates;
    uint8_t width, height;
} room_t;

typedef struct dungeon {
    character_t *pc;
    uint8_t num_monsters;

    heap_t event_queue;
    event_t *events;
    character_t *character_map[DUNGEON_Y][DUNGEON_X];
    character_t *characters;

    uint8_t non_tunnel_distance_map[DUNGEON_Y][DUNGEON_X];
    uint8_t tunnel_distance_map[DUNGEON_Y][DUNGEON_X];
    terrain_t map[DUNGEON_Y][DUNGEON_X];
    uint8_t hardness_map[DUNGEON_Y][DUNGEON_X];

    uint16_t num_rooms;
    room_t *rooms;
    uint16_t num_stairs_up;
    point_t *stairs_up;
    uint16_t num_stairs_down;
    point_t *stairs_down;
} dungeon_t;

void render_dungeon(dungeon_t *);
void render_hardness_map(dungeon_t *);
void render_movement_cost_map(dungeon_t *);
void render_distance_map(dungeon_t *);
void render_tunnel_distance_map(dungeon_t *);

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

bool intersects(room_t *, room_t *);
bool out_of_bounds(room_t *, int, int);
room_t * get_room(dungeon_t *, point_t *);
point_t get_valid_point(dungeon_t *, bool);

#endif
