#ifndef DUNGEON_H
# define DUNGEON_H

#include <stdint.h>
#include <stdbool.h>

#define DUNGEON_X   80
#define DUNGEON_Y   21
#define MIN_ROOMS   6
#define MAX_ROOMS   7
#define ROOM_MIN_X  4
#define ROOM_MIN_Y  3
#define ROOM_MAX_X  15
#define ROOM_MAX_Y  10
#define STAIRS_MAX  3

typedef enum __attribute__ ((__packed__)) terrain_type {
  ter_player,
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

typedef struct point {
    uint8_t x, y;
} point_t;

typedef struct room {
    point_t coordinates;
    uint8_t width, height;
} room_t;

typedef struct dungeon {
    point_t pc_coordinates;
    uint8_t pc_distance[DUNGEON_Y][DUNGEON_X];
    uint8_t pc_tunnel[DUNGEON_Y][DUNGEON_X];

    terrain_t map[DUNGEON_Y][DUNGEON_X];
    uint8_t hardness[DUNGEON_Y][DUNGEON_X];
    
    uint16_t num_rooms;
    room_t *rooms;

    uint16_t num_stairs_up;
    point_t *stairs_up;

    uint16_t num_stairs_down;
    point_t *stairs_down;
} dungeon_t;

int render(dungeon_t *);
bool contains(int, char *[], char *);
int deep_free_dungeon(dungeon_t *);

int path_init(char **);
int read_dungeon_from_disk(dungeon_t *, char *);
int write_dungeon_to_disk(dungeon_t *, char *);

int generate(dungeon_t *);
int generate_border(dungeon_t *);
int generate_rooms(dungeon_t *);
int generate_corridors(dungeon_t *);
int generate_staircases(dungeon_t *);
int generate_hardness(dungeon_t *);
int generate_terrain(dungeon_t *);
bool intersects(room_t *, room_t *);
bool out_of_bounds(room_t *, int, int);

#endif
