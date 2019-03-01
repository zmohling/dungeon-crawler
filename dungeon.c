#include <endian.h>
#include <errno.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dungeon.h"
#include "heap.h"
#include "path_finder.h"

int render_dungeon(dungeon_t *d) {
    int i, j;
    for (i = 0; i < DUNGEON_Y; i++) {
        for (j = 0; j < DUNGEON_X; j++) {
            char c;

            /* Border */
            if (i == 0 || i == (DUNGEON_Y - 1)) {
                c = '-';
            } else if (j == 0 || j == (DUNGEON_X - 1)) {
                c = '|';
            }

            /* Characters */
            character_t *character = d->character_map[i][j];
            if (character != NULL) {
                if (character->is_pc == true) {
                    printf("%c", character->symbol);
                } else {
                    printf("%x", d->character_map[i][j]->symbol & 0xff);
                }

                continue;
            }

            /* Terrain */
            terrain_t t = d->map[i][j];
            switch (t) {
                case ter_wall_immutable:
                    break;
                case ter_wall:
                    c = ' ';
                    break;
                case ter_floor_room:
                    c = '.';
                    break;
                case ter_floor_hall:
                    c = '#';
                    break;
                case ter_stairs_up:
                    c = '<';
                    break;
                case ter_stairs_down:
                    c = '>';
                    break;
                default:
                    c = '+';
            }

            printf("%c", c);
        }

        printf("\n");
    }

    return 0;
}

/*
 * Free allocated memory in
 * dungeon struct.
 */
int deep_free_dungeon(dungeon_t *d) {
    free(d->rooms);
    free(d->stairs_up);
    free(d->stairs_down);
    // free(d->characters);
    // free(d->events);

    int i;
    for (i = 0; i < (d->num_monsters + 1); i++) {
        character_t *c = &(d->characters[i]);
        if (c->npc != NULL) {
            free(c->npc);
        }
    }

    return 0;
}

/*
 * Generate the random dungeon.
 */
int generate_dungeon(dungeon_t *d) {
    generate_border(d);
    generate_rooms(d);
    generate_corridors(d);
    generate_staircases(d);
    generate_hardness(d);

    return 0;
}

/*
 * Generate border around edges and initialize
 * dungeon array to spaces.
 * char *dungeon: Dungeon array to write to.
 */
int generate_border(dungeon_t *d) {
    int i, j;
    for (i = 0; i < DUNGEON_Y; i++) {
        for (j = 0; j < DUNGEON_X; j++) {
            d->map[i][j] = ter_wall;

            if (j == 0 || j == (DUNGEON_X - 1)) {
                d->map[i][j] = ter_wall_immutable;
            }

            if (i == 0 || i == (DUNGEON_Y - 1)) {
                d->map[i][j] = ter_wall_immutable;
            }
        }
    }
    return 0;
}

/*
 * Generates rooms for the dungeon to fit the
 * given parameters.
 * char *dungeon: Dungeon array to write to.
 * Room *rooms: Room array to populate.
 * int num_rooms: Number of rooms to generate.
 * int x: Size of dungeon in the x direction.
 * int y: Size of dungeon in the y direction.
 */
int generate_rooms(dungeon_t *d) {
    int attempts_to_generate = 0;

    d->num_rooms = rand() % (MAX_ROOMS - MIN_ROOMS + 1) + MIN_ROOMS;
    d->rooms = malloc(d->num_rooms * sizeof(room_t));

    uint8_t i;
    for (i = 0; i < d->num_rooms; i++) {
        bool _intersects = false;
        bool _out_of_bounds = false;
        do {
            /* Generate random room within bounds and margin of 2 */
            d->rooms[i].coordinates.x = rand() % (DUNGEON_X - 4) + 2;
            d->rooms[i].coordinates.y = rand() % (DUNGEON_Y - 4) + 2;

            d->rooms[i].width = rand() % (ROOM_MAX_X - 4) + 4;
            d->rooms[i].height = rand() % (ROOM_MAX_Y - 3) + 3;

            int j;
            for (j = (i - 1); j >= 0; j--) {
                if ((_intersects =
                         intersects(&(d->rooms[i]), &(d->rooms[j])))) {
                    break;
                }
            }

            _out_of_bounds =
                out_of_bounds(&(d->rooms[i]), (DUNGEON_X - 1), (DUNGEON_Y - 1));

            if (attempts_to_generate > 2000) {
                fprintf(stderr, "Error: Could not generate rooms.");
                exit(-1);
            }
            attempts_to_generate++;
        } while (_intersects ||
                 _out_of_bounds);  // continue generating until the random room
                                   // is in bounds not non-intersecting.

        /* Write generated room to dungeon array */
        int x, y;
        for (y = d->rooms[i].coordinates.y;
             y < (d->rooms[i].coordinates.y + d->rooms[i].height); y++) {
            for (x = d->rooms[i].coordinates.x;
                 x < (d->rooms[i].coordinates.x + d->rooms[i].width); x++) {
                d->map[y][x] = ter_floor_room;
            }
        }
    }
    return 0;
}

/*
 * Returns 1 (true) if the specified room is out of bounds.
 * Room room: Room which to check if in bounds.
 * int bound_x: Index of bound in the x direction.
 * int bound_y: Index of bound in the y direction.
 */
bool out_of_bounds(room_t *room, int bound_x, int bound_y) {
    int room_x1 = room->coordinates.x;
    int room_y1 = room->coordinates.y;
    int room_x2 = room_x1 + room->width;
    int room_y2 = room_y1 + room->height;

    if (room_x1 >= 2 && room_y1 >= 2 && room_x2 <= 77 && room_y2 <= 18) {
        return false;
    }

    return true;
}

/*
 * Returns 1 (true) if the two specified rooms are intersecting.
 * Room a: First room of the pair.
 * Room b: Second room of the pair.
 */
bool intersects(room_t *a, room_t *b) {
    int a_x1 = a->coordinates.x;
    int a_y1 = a->coordinates.y;
    int a_x2 = a_x1 + a->width;
    int a_y2 = a_y1 + a->height;

    int b_x1 = b->coordinates.x;
    int b_y1 = b->coordinates.y;
    int b_x2 = b_x1 + b->width;
    int b_y2 = b_y1 + b->height;

    if (a_x1 > b_x2 + 1 || a_x2 < b_x1 - 1 || a_y1 > b_y2 + 1 ||
        a_y2 < b_y1 - 1) {
        return false;
    }

    return true;
}

/*
 * Generate corridors between rooms.
 * char *dungeon: Dungeon array to write to.
 * Room *rooms: Room array.
 * int num_rooms: Number of rooms in *rooms
 */
int generate_corridors(dungeon_t *d) {
    room_t connected_rooms[d->num_rooms];
    connected_rooms[0] = d->rooms[0];

    int i;
    for (i = 1; i < d->num_rooms; i++) {
        room_t closest_room;

        /* Find the closest room using Euclidean distance formula */
        int j;
        for (j = (i - 1); j >= 0; j--) {
            if (j == (i - 1)) {
                closest_room = connected_rooms[j];
            } else {
                int distance = sqrt(pow((d->rooms[i].coordinates.x -
                                         connected_rooms[j].coordinates.x),
                                        2) +
                                    pow((d->rooms[i].coordinates.y -
                                         connected_rooms[j].coordinates.y),
                                        2));

                int closest_distance = sqrt(pow((d->rooms[i].coordinates.x -
                                                 closest_room.coordinates.x),
                                                2) +
                                            pow((d->rooms[i].coordinates.y -
                                                 closest_room.coordinates.y),
                                                2));

                if (distance < closest_distance) {
                    closest_room = connected_rooms[j];
                }
            }
        }

        /* Write corridors to dungeon */
        int x = (i % 2 == 0) ? d->rooms[i].coordinates.x
                             : d->rooms[i].coordinates.x + d->rooms[i].width -
                                   1;  // add randomness by writing corridor
                                       // from second point on even indexes

        int y = (i % 2 == 0)
                    ? d->rooms[i].coordinates.y
                    : d->rooms[i].coordinates.y + d->rooms[i].height - 1;

        int x_closest =
            (i % 2 == 0) ? closest_room.coordinates.x
                         : closest_room.coordinates.x + closest_room.width - 1;

        int y_closest =
            (i % 2 == 0) ? closest_room.coordinates.y
                         : closest_room.coordinates.y + closest_room.height - 1;

        while (x != x_closest) {
            int direction = (x - x_closest) / abs(x - x_closest);
            x = x - (direction * 1);

            d->map[y][x] = (d->map[y][x] == ter_floor_room) ? ter_floor_room
                                                            : ter_floor_hall;
        }

        while (y != y_closest) {
            int direction = (y - y_closest) / abs(y - y_closest);
            y = y - (direction * 1);

            d->map[y][x] = (d->map[y][x] == ter_floor_room) ? ter_floor_room
                                                            : ter_floor_hall;
        }

        connected_rooms[i] = d->rooms[i];
    }
    return 0;
}

/*
 * Generate 1-2 up and down staircases in
 * rooms or corridors.
 * char *dungeon: Dungeon array to write to.
 * int x: Size of dungeon in x direction.
 * int y: Size of dungeon in y direction.
 */
int generate_staircases(dungeon_t *d) {
    d->num_stairs_up = rand() % STAIRS_MAX + 1;
    d->stairs_up = malloc(d->num_stairs_up * sizeof(point_t));

    int i;
    for (i = 0; i < d->num_stairs_up;) {
        int _x = rand() % DUNGEON_X;
        int _y = rand() % DUNGEON_Y;

        terrain_t *p;
        p = &(d->map[_y][_x]);

        if (*p == ter_floor_hall || *p == ter_floor_room) {
            point_t coord = {_x, _y};
            d->stairs_up[i] = coord;

            *p = ter_stairs_up;
            i++;
        }
    }

    d->num_stairs_down = rand() % STAIRS_MAX + 1;
    d->stairs_down = malloc(d->num_stairs_down * sizeof(point_t));

    int j;
    for (j = 0; j < d->num_stairs_down;) {
        int _x = rand() % DUNGEON_X;
        int _y = rand() % DUNGEON_Y;

        terrain_t *p;
        p = &(d->map[_y][_x]);

        if (*p == ter_floor_hall || *p == ter_floor_room) {
            point_t coord = {_x, _y};
            d->stairs_down[j] = coord;

            *p = ter_stairs_down;
            j++;
        }
    }
    return 0;
}

/*
 * Generates hardness array.
 */
int generate_hardness(dungeon_t *d) {
    int i, j;
    for (i = 0; i < DUNGEON_Y; i++) {
        for (j = 0; j < DUNGEON_X; j++) {
            terrain_t t = d->map[i][j];
            if (t == ter_wall) {
                d->hardness_map[i][j] = rand() % 254 + 1;
            } else if (t == ter_wall_immutable) {
                d->hardness_map[i][j] = 255;
            } else {
                d->hardness_map[i][j] = 0;
            }
        }
    }
    return 0;
}

/*
 * Generate terrain for dungeons
 * which have been read from disk.
 */
int generate_terrain(dungeon_t *d) {
    /* Generate immutable border and walls */
    generate_border(d);

    /* Generate room terrain */
    int i, j;
    for (i = 0; i < d->num_rooms; i++) {
        int x, y;
        for (y = d->rooms[i].coordinates.y;
             y < (d->rooms[i].coordinates.y + d->rooms[i].height); y++) {
            for (x = d->rooms[i].coordinates.x;
                 x < (d->rooms[i].coordinates.x + d->rooms[i].width); x++) {
                d->map[y][x] = ter_floor_room;
            }
        }
    }

    /* Generate up stairs terrain */
    for (i = 0; i < d->num_stairs_up; i++) {
        d->map[d->stairs_up[i].y][d->stairs_up[i].x] = ter_stairs_up;
    }

    /* Generate down stairs terrain */
    for (i = 0; i < d->num_stairs_down; i++) {
        d->map[d->stairs_down[i].y][d->stairs_down[i].x] = ter_stairs_down;
    }

    /* Generate corridor terrain */
    for (i = 0; i < DUNGEON_Y; i++) {
        for (j = 0; j < DUNGEON_X; j++) {
            terrain_t *t = &(d->map[i][j]);

            if (j == 0 || j == (DUNGEON_X - 1)) {
                *t = ter_wall_immutable;
            }

            if (i == 0 || i == (DUNGEON_Y - 1)) {
                *t = ter_wall_immutable;
            }

            uint8_t h = d->hardness_map[i][j];

            if (h == 0 && *t == ter_wall) {
                *t = ter_floor_hall;
            }
        }
    }

    /* pc.position */
    // d->map[d->pc.position.y][d->pc->position.x] = ter_player;

    return 0;
}

point_t get_valid_point(dungeon_t *d) {
    point_t p;

    do {
        uint32_t rand_y = rand() % (DUNGEON_Y - 3) + 2;
        uint32_t rand_x = rand() % (DUNGEON_X + 3) + 2;

        if (d->hardness_map[rand_y][rand_x] == 0 &&
            d->character_map[rand_y][rand_x] == NULL &&
            d->map[rand_y][rand_x] != ter_wall &&
            d->map[rand_y][rand_x] != ter_wall_immutable) {
            p.x = rand_x;
            p.y = rand_y;

            break;
        }

    } while (true);

    return p;
}

int read_dungeon_from_disk(dungeon_t *d, char *path) {
    FILE *f;
    if ((f = fopen(path, "r")) == NULL) {
        fprintf(stderr, "Error: could not open dungeon file. (%d)\n", errno);
        exit(-4);
    }

    char file_type_marker[12];
    if (!fread(file_type_marker, 12, 1, f)) {
        fprintf(stderr, "Error: could not load <file_type_marker>. (%d)\n",
                errno);
        exit(-6);
    }

    uint32_t file_version;
    if (!fread(&file_version, sizeof(uint32_t), 1, f)) {
        fprintf(stderr, "Error: could not load <file_version>. (%d)\n", errno);
        exit(-7);
    }
    file_version = be32toh(file_version);

    uint32_t file_size;
    if (!fread(&file_size, sizeof(uint32_t), 1, f)) {
        fprintf(stderr, "Error: could not load <file_size>. (%d)\n", errno);
        exit(-8);
    }
    file_size = be32toh(file_size);

    /* pc.position */
    if (!fread(&(d->pc->position.x), sizeof(char), 1, f)) {
        fprintf(stderr, "Error: could not load <pc.position.x>. (%d)\n", errno);
        exit(-9);
    }
    if (!fread(&(d->pc->position.y), sizeof(char), 1, f)) {
        fprintf(stderr, "Error: could not load <pc.position.y>. (%d)\n", errno);
        exit(-10);
    }

    /* hardness_map */
    int i, j;
    for (i = 0; i < DUNGEON_Y; i++) {
        for (j = 0; j < DUNGEON_X; j++) {
            if (!fread(&(d->hardness_map[i][j]), sizeof(uint8_t), 1, f)) {
                fprintf(stderr,
                        "Error: could not load <hardness_map[%d][%d]>. (%d)\n",
                        i, j, errno);
                exit(-11);
            }
        }
    }

    /* num_rooms */
    uint16_t be_num_rooms;
    if (!fread(&be_num_rooms, sizeof(uint16_t), 1, f)) {
        fprintf(stderr, "Error: could not load <num_rooms>. (%d)\n", errno);
        exit(-12);
    }
    d->num_rooms = be16toh(be_num_rooms);

    /* rooms array*/
    if (!(d->rooms = malloc(d->num_rooms * sizeof(room_t)))) {
        fprintf(stderr, "Error: could not allocate memory for <*rooms>. (%d)\n",
                errno);
        exit(-13);
    }

    for (i = 0; i < d->num_rooms; i++) {
        if (!fread(&(d->rooms[i].coordinates.x), sizeof(uint8_t), 1, f)) {
            fprintf(stderr,
                    "Error: could not load <d->rooms[i].coordinates.x>. (%d)\n",
                    errno);
            exit(-14);
        }

        if (!fread(&(d->rooms[i].coordinates.y), sizeof(uint8_t), 1, f)) {
            fprintf(stderr,
                    "Error: could not load <d->rooms[i].coordinates.y>. (%d)\n",
                    errno);
            exit(-15);
        }

        if (!fread(&(d->rooms[i].width), sizeof(uint8_t), 1, f)) {
            fprintf(stderr, "Error: could not load <d->rooms[i].width>. (%d)\n",
                    errno);
            exit(-16);
        }

        if (!fread(&(d->rooms[i].height), sizeof(uint8_t), 1, f)) {
            fprintf(stderr,
                    "Error: could not load <d->rooms[i].height>. (%d)\n",
                    errno);
            exit(-17);
        }
    }

    /* num_stairs_up */
    uint16_t be_num_stairs_up;
    if (!fread(&be_num_stairs_up, sizeof(uint16_t), 1, f)) {
        fprintf(stderr, "Error: could not load <num_stairs_up>. (%d)\n", errno);
        exit(-18);
    }
    d->num_stairs_up = be16toh(be_num_stairs_up);

    /* stairs_up */
    if (!(d->stairs_up = malloc(d->num_stairs_up * sizeof(point_t)))) {
        fprintf(stderr,
                "Error: could not allocate memory for <*stairs_up>. (%d)\n",
                errno);
        exit(-19);
    }

    for (i = 0; i < d->num_stairs_up; i++) {
        if (!fread(&(d->stairs_up[i].x), sizeof(uint8_t), 1, f)) {
            fprintf(stderr, "Error: could not load <d->stairs_up[i].x>. (%d)\n",
                    errno);
            exit(-20);
        }

        if (!fread(&(d->stairs_up[i].y), sizeof(uint8_t), 1, f)) {
            fprintf(stderr, "Error: could not load <d->stairs_up[i].y>. (%d)\n",
                    errno);
            exit(-21);
        }
    }

    /* num_stairs_down */
    uint16_t be_num_stairs_down;
    if (!fread(&be_num_stairs_down, sizeof(uint16_t), 1, f)) {
        fprintf(stderr, "Error: could not load <num_stairs_down>. (%d)\n",
                errno);
        exit(-22);
    }
    d->num_stairs_down = be16toh(be_num_stairs_down);

    /* stairs_up */
    if (!(d->stairs_down = malloc(d->num_stairs_down * sizeof(point_t)))) {
        fprintf(stderr,
                "Error: could not allocate memory for <*stairs_down>. (%d)\n",
                errno);
        exit(-23);
    }

    for (i = 0; i < d->num_stairs_down; i++) {
        if (!fread(&(d->stairs_down[i].x), sizeof(uint8_t), 1, f)) {
            fprintf(stderr,
                    "Error: could not load <d->stairs_down[i].x>. (%d)\n",
                    errno);
            exit(-24);
        }

        if (!fread(&(d->stairs_down[i].y), sizeof(uint8_t), 1, f)) {
            fprintf(stderr,
                    "Error: could not load <d->stairs_down[i].y>. (%d)\n",
                    errno);
            exit(-25);
        }
    }

    fclose(f);

    return 0;
}

int write_dungeon_to_disk(dungeon_t *d, char *path) {
    FILE *f;
    if ((f = fopen(path, "w")) == NULL) {
        fprintf(stderr, "Error: could not open dungeon file. (%d)\n", errno);
        exit(-34);
    }

    char file_type_marker[12] = {'R', 'L', 'G', '3', '2', '7',
                                 '-', 'S', '2', '0', '1', '9'};
    if (!fwrite(file_type_marker, sizeof(file_type_marker), 1, f)) {
        fprintf(stderr, "Error: could not save <file_type_marker>. (%d)\n",
                errno);
        exit(-36);
    }

    uint32_t temp = 0;
    uint32_t file_version = htobe32(temp);
    if (!fwrite(&file_version, sizeof(uint32_t), 1, f)) {
        fprintf(stderr, "Error: could not save <file_version>. (%d)\n", errno);
        exit(-37);
    }

    temp =
        (sizeof(file_type_marker) + sizeof(uint32_t) * 2 + sizeof(uint8_t) * 2 +
         sizeof(uint8_t) * (DUNGEON_Y * DUNGEON_X) + sizeof(uint16_t) * 3 +
         sizeof(uint8_t) * d->num_rooms + sizeof(uint8_t) * d->num_stairs_up +
         sizeof(uint8_t) * d->num_stairs_down);

    uint32_t file_size = htobe32(temp);
    if (!fwrite(&file_size, sizeof(uint32_t), 1, f)) {
        fprintf(stderr, "Error: could not save <file_size>. (%d)\n", errno);
        exit(-38);
    }

    /* pc.position */
    if (!fwrite(&(d->pc->position.x), sizeof(char), 1, f)) {
        fprintf(stderr, "Error: could not save <pc.position.x>. (%d)\n", errno);
        exit(-39);
    }
    if (!fwrite(&(d->pc->position.y), sizeof(char), 1, f)) {
        fprintf(stderr, "Error: could not save <pc.position.y>. (%d)\n", errno);
        exit(-40);
    }

    /* hardness_map */
    int i, j;
    for (i = 0; i < DUNGEON_Y; i++) {
        for (j = 0; j < DUNGEON_X; j++) {
            if (!fwrite(&(d->hardness_map[i][j]), sizeof(uint8_t), 1, f)) {
                fprintf(stderr,
                        "Error: could not save <hardness_map[%d][%d]>. (%d)\n",
                        i, j, errno);
                exit(-41);
            }
        }
    }

    /* num_rooms */
    uint16_t num_rooms = d->num_rooms;
    uint16_t be_num_rooms = htobe16(num_rooms);
    if (!fwrite(&be_num_rooms, sizeof(uint16_t), 1, f)) {
        fprintf(stderr, "Error: could not save <num_rooms>. (%d)\n", errno);
        exit(-42);
    }

    /* rooms array*/
    for (i = 0; i < d->num_rooms; i++) {
        if (!fwrite(&(d->rooms[i].coordinates.x), sizeof(uint8_t), 1, f)) {
            fprintf(stderr,
                    "Error: could not save <d->rooms[i].coordinates.x>. (%d)\n",
                    errno);
            exit(-44);
        }

        if (!fwrite(&(d->rooms[i].coordinates.y), sizeof(uint8_t), 1, f)) {
            fprintf(stderr,
                    "Error: could not save <d->rooms[i].coordinates.y>. (%d)\n",
                    errno);
            exit(-45);
        }

        if (!fwrite(&(d->rooms[i].width), sizeof(uint8_t), 1, f)) {
            fprintf(stderr, "Error: could not save <d->rooms[i].width>. (%d)\n",
                    errno);
            exit(-46);
        }

        if (!fwrite(&(d->rooms[i].height), sizeof(uint8_t), 1, f)) {
            fprintf(stderr,
                    "Error: could not save <d->rooms[i].height>. (%d)\n",
                    errno);
            exit(-47);
        }
    }

    /* num_stairs_up */
    uint16_t num_stairs_up = d->num_stairs_up;
    uint16_t be_num_stairs_up = htobe16(num_stairs_up);
    if (!fwrite(&be_num_stairs_up, sizeof(uint16_t), 1, f)) {
        fprintf(stderr, "Error: could not save <num_stairs_up>. (%d)\n", errno);
        exit(-48);
    }

    /* stairs_up */
    for (i = 0; i < d->num_stairs_up; i++) {
        if (!fwrite(&(d->stairs_up[i].x), sizeof(uint8_t), 1, f)) {
            fprintf(stderr, "Error: could not save <d->stairs_up[i].x>. (%d)\n",
                    errno);
            exit(-50);
        }

        if (!fwrite(&(d->stairs_up[i].y), sizeof(uint8_t), 1, f)) {
            fprintf(stderr, "Error: could not save <d->stairs_up[i].y>. (%d)\n",
                    errno);
            exit(-51);
        }
    }

    /* num_stairs_down */
    uint16_t num_stairs_down = d->num_stairs_down;
    uint16_t be_num_stairs_down = htobe16(num_stairs_down);
    if (!fwrite(&be_num_stairs_down, sizeof(uint16_t), 1, f)) {
        fprintf(stderr, "Error: could not save <num_stairs_down>. (%d)\n",
                errno);
        exit(-52);
    }

    /* stairs_up */
    for (i = 0; i < d->num_stairs_down; i++) {
        if (!fwrite(&(d->stairs_down[i].x), sizeof(uint8_t), 1, f)) {
            fprintf(stderr,
                    "Error: could not save <d->stairs_down[i].x>. (%d)\n",
                    errno);
            exit(-54);
        }

        if (!fwrite(&(d->stairs_down[i].y), sizeof(uint8_t), 1, f)) {
            fprintf(stderr,
                    "Error: could not save <d->stairs_down[i].y>. (%d)\n",
                    errno);
            exit(-55);
        }
    }

    fclose(f);

    return 0;
}
