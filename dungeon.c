#include <endian.h>
#include <errno.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#include "dungeon.h"
#include "heap.h"
#include "pathfinder.h"

int main(int argc, char *argv[]) {
    /* Check arguments */
    if (argc > 3) {
        fprintf(stderr, "Ussage: %s [--save][--load]\n", argv[0]);
        exit(-1);
    }

    /* Initialize path to ~/.rlg327/ */
    char *path;
    path_init(&path);

    /* Dungeon*/
    dungeon_t dungeon;

    /* Load switch */
    char *load_switch_str = "--load";
    if (contains(argc, argv, load_switch_str)) {
        read_dungeon_from_disk(&dungeon, path);
        generate_terrain(&dungeon);
    } else {
        generate(&dungeon);
        dungeon.pc_coordinates.x = dungeon.rooms[0].coordinates.x;
        dungeon.pc_coordinates.y = dungeon.rooms[0].coordinates.y;
        dungeon.map[dungeon.pc_coordinates.y][dungeon.pc_coordinates.x] =
            ter_player;
    }

    /* Render */
    render(&dungeon);
    non_tunnel_distance_map(&dungeon);
    tunnel_distance_map(&dungeon);

    /* Render Distance maps - temporary */
    int i, j;
    for (i = 0; i < DUNGEON_Y; i++) {
        for (j = 0; j < DUNGEON_X; j++) {
            uint8_t temp = dungeon.non_tunnel_distance_map[i][j] % 10;
            if (i == dungeon.pc_coordinates.y &&
                j == dungeon.pc_coordinates.x) {
                printf("@");
            } else if (dungeon.non_tunnel_distance_map[i][j] < UINT8_MAX) {
                printf("%d", temp);
            } else {
                printf(" ");
            }
        }
        printf("\n");
    }

    for (i = 0; i < DUNGEON_Y; i++) {
        for (j = 0; j < DUNGEON_X; j++) {
            uint8_t temp = dungeon.tunnel_distance_map[i][j] % 10;
            if (i == dungeon.pc_coordinates.y &&
                j == dungeon.pc_coordinates.x) {
                printf("@");
            } else if (dungeon.tunnel_distance_map[i][j] < UINT8_MAX) {
                printf("%d", temp);
            } else {
                printf("X");
            }
        }
        printf("\n");
    }

    /* Save switch */
    char *save_str = "--save";
    if (contains(argc, argv, save_str)) {
        write_dungeon_to_disk(&dungeon, path);
    }

    free(path);
    deep_free_dungeon(&dungeon);

    return 0;
}

int render(dungeon_t *d) {
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
                case ter_player:
                    c = '@';
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
 * Iterates through *argv for specified
 * switch *s. Returns true if *argv contains
 * the string *s.
 */
bool contains(int argc, char *argv[], char *s) {
    int i;
    for (i = 0; i < argc; i++) {
        if (!(strcmp(argv[i], s))) {
            return true;
        }
    }

    return false;
}

/*
 * Free allocated memory in
 * dungeon struct.
 */
int deep_free_dungeon(dungeon_t *d) {
    free(d->rooms);
    free(d->stairs_up);
    free(d->stairs_down);

    return 0;
}

/*
 * Generate the random dungeon.
 */
int generate(dungeon_t *d) {
    int seed = time(NULL);
    srand(seed);

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

            if (j == 0 || j == 79) {
                d->map[i][j] = ter_wall_immutable;
            }

            if (i == 0 || i == 20) {
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
            if (t == ter_wall || t == ter_wall_immutable) {
                d->hardness[i][j] = rand() % 254 + 1;
            } else {
                d->hardness[i][j] = 0;
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

            if (j == 0 || j == 79) {
                *t = ter_wall_immutable;
            }

            if (i == 0 || i == 20) {
                *t = ter_wall_immutable;
            }

            uint8_t h = d->hardness[i][j];

            if (h == 0 && *t == ter_wall) {
                *t = ter_floor_hall;
            }
        }
    }

    /* pc_coordinates */
    d->map[d->pc_coordinates.y][d->pc_coordinates.x] = ter_player;

    return 0;
}

/*
 * Initialize path to ~/.rlg327/ and
 * make it if not already existent. Returns
 * complete path the dungeon file.
 */
int path_init(char **path) {
    /* FS setup */
    char *home = getenv("HOME");
    char *dir = malloc(strlen(home) + strlen("/.rlg327/") + 1);
    strcpy(dir, home);
    strcat(dir, "/.rlg327/");

    if (mkdir(dir, 0755)) {
        /* errno == 17 is an existing directory */
        if (errno != 17) {
            fprintf(stderr,
                    "Error: ~/.rlg327/ could not be found nor created. (%d)\n",
                    errno);
            exit(-1);
        }
    }

    /* Dot file directory's path */
    //*path = malloc(strlen(home) + strlen("/.rlg327/dungeon") + 1);
    *path = malloc(
        strlen(home) +
        strlen("/courses/com-s-327/.extra_files/saved_dungeons/01.rlg327") + 1);
    strcpy(*path, home);
    strcat(*path, "/courses/com-s-327/.extra_files/saved_dungeons/01.rlg327");

    free(dir);

    return 0;
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

    /* pc_coordinates */
    if (!fread(&(d->pc_coordinates.x), sizeof(char), 1, f)) {
        fprintf(stderr, "Error: could not load <pc_coordinates.x>. (%d)\n",
                errno);
        exit(-9);
    }
    if (!fread(&(d->pc_coordinates.y), sizeof(char), 1, f)) {
        fprintf(stderr, "Error: could not load <pc_coordinates.y>. (%d)\n",
                errno);
        exit(-10);
    }

    /* hardness */
    int i, j;
    for (i = 0; i < DUNGEON_Y; i++) {
        for (j = 0; j < DUNGEON_X; j++) {
            if (!fread(&(d->hardness[i][j]), sizeof(uint8_t), 1, f)) {
                fprintf(stderr,
                        "Error: could not load <hardness[%d][%d]>. (%d)\n", i,
                        j, errno);
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

    /* pc_coordinates */
    if (!fwrite(&(d->pc_coordinates.x), sizeof(char), 1, f)) {
        fprintf(stderr, "Error: could not save <pc_coordinates.x>. (%d)\n",
                errno);
        exit(-39);
    }
    if (!fwrite(&(d->pc_coordinates.y), sizeof(char), 1, f)) {
        fprintf(stderr, "Error: could not save <pc_coordinates.y>. (%d)\n",
                errno);
        exit(-40);
    }

    /* hardness */
    int i, j;
    for (i = 0; i < DUNGEON_Y; i++) {
        for (j = 0; j < DUNGEON_X; j++) {
            if (!fwrite(&(d->hardness[i][j]), sizeof(uint8_t), 1, f)) {
                fprintf(stderr,
                        "Error: could not save <hardness[%d][%d]>. (%d)\n", i,
                        j, errno);
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
