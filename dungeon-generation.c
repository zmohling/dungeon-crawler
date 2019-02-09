#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdint.h>
#include <errno.h>

#include "dungeon-generation.h"
#include "dungeon.h"
/*
dungeon_t * generate(void);
void generate_border(char *);
void generate_rooms(char *, Room *, int, int, int);
void generate_corridors(char *, Room *, int);
void generate_staircases(char *, int, int);
int intersects(Room, Room);
int out_of_bounds(Room, int, int);
*/
/*
int main(int argc, char *argv[])
{
    generate();

    return 0;
}
*/
/*
 * Generate and draw a dungeon.
 */
int generate(dungeon_t *d)
{
    int seed = time(NULL);
    srand(seed);
    //printf("Seed: %d\n", seed);
   
    //memset(d->map, 0, (DUNGEON_Y * DUNGEON_X));

    //static char *dungeon;
    //dungeon = (char *) calloc(x * y, sizeof(char));
    //char dungeon[y][x];
    //memset(dungeon, ' ', y * x * sizeof(char));

    generate_border(d);
    generate_rooms(d);
    generate_corridors(d);
    generate_staircases(d);
    generate_hardness(d);

    /*
    int i, j;
    for (i = 0; i < DUNGEON_Y; i++)
    {
        for (j = 0; j < DUNGEON_X; j++)
        {
            if (d->hardness[i][j] == 0)
            {
                printf("%c", '#');
            } else {
                printf("%c", ' '); 
            }
        }

        printf("\n");
    }
    */

    return 0;
    //free(dungeon);
}

/*
 * Generate border around edges and initialize 
 * dungeon array to spaces.
 * char *dungeon: Dungeon array to write to.
 */
int generate_border(dungeon_t *d)
{
    int i, j;
    for (i = 0; i < DUNGEON_Y; i++)
    {
        for (j = 0; j < DUNGEON_X; j++)
        {
            d->map[i][j] = ter_wall; 

            if (j == 0 || j == 79)
            {
                d->map[i][j] = ter_wall_immutable; 
            }

            if (i == 0 || i == 20)
            {
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
int generate_rooms(dungeon_t *d)
{
    int attempts_to_generate = 0;

    d->num_rooms = rand() % (MAX_ROOMS - MIN_ROOMS + 1) + MIN_ROOMS;
    d->rooms = malloc(d->num_rooms * sizeof(room_t));

    uint8_t i;
    for (i = 0; i < d->num_rooms; i++)
    {
        bool _intersects = false;
        bool _out_of_bounds = false;
        do {

            /* Generate random room within bounds and margin of 2 */
            d->rooms[i].coordinates.x = rand() % (DUNGEON_X - 4) + 2;
            d->rooms[i].coordinates.y = rand() % (DUNGEON_Y - 4) + 2;

            d->rooms[i].width = rand() % (ROOM_MAX_X - 4) + 4;
            d->rooms[i].height = rand() % (ROOM_MAX_Y - 3) + 3;

            int j;
            for (j = (i - 1); j >= 0; j--)
            {
               if ((_intersects = intersects(&(d->rooms[i]), &(d->rooms[j]))))
               {
                   break;
               }
            }

            _out_of_bounds = out_of_bounds(&(d->rooms[i]), (DUNGEON_X - 1), (DUNGEON_Y- 1));

            if (attempts_to_generate > 2000)
            {
                fprintf(stderr, "Error: Could not generate rooms.");
                exit(-1);
            }
            attempts_to_generate++;
        } while(_intersects || _out_of_bounds); // continue generating until the random room is in bounds not non-intersecting.

        /* Write generated room to dungeon array */
        int x, y;
        for (y = d->rooms[i].coordinates.y; y < (d->rooms[i].coordinates.y + d->rooms[i].height); y++)
        {
            for (x = d->rooms[i].coordinates.x; x < (d->rooms[i].coordinates.x + d->rooms[i].width); x++)
            {
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
bool out_of_bounds(room_t *room, int bound_x, int bound_y)
{
    int room_x1 = room->coordinates.x;
    int room_y1 = room->coordinates.y;
    int room_x2 = room_x1 + room->width;
    int room_y2 = room_y1 + room->height;
    
    if (room_x1 >= 2 && room_y1 >= 2 && room_x2 <= 77 && room_y2 <= 18)
    {
        return false;
    }

    return true;
}

/*
 * Returns 1 (true) if the two specified rooms are intersecting. 
 * Room a: First room of the pair.
 * Room b: Second room of the pair.
 */
bool intersects(room_t *a, room_t *b)
{
    int a_x1 = a->coordinates.x;
    int a_y1 = a->coordinates.y;
    int a_x2 = a_x1 + a->width;
    int a_y2 = a_y1 + a->height;
    
    int b_x1 = b->coordinates.x;
    int b_y1 = b->coordinates.y;
    int b_x2 = b_x1 + b->width;
    int b_y2 = b_y1 + b->height;

    if (a_x1 > b_x2 + 1 || a_x2 < b_x1 - 1 || a_y1 > b_y2 + 1 || a_y2 < b_y1 - 1)
    {
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
int generate_corridors(dungeon_t *d)
{
    room_t connected_rooms[d->num_rooms];
    connected_rooms[0] = d->rooms[0];

    int i;
    for (i = 1; i < d->num_rooms; i++)
    {
        room_t closest_room; 

        /* Find the closest room using Euclidean distance formula */
        int j;
        for (j = (i - 1); j >= 0; j--)
        {
            if (j == (i - 1))
            {
                closest_room = connected_rooms[j];
            } else {
                int distance = sqrt(pow((d->rooms[i].coordinates.x - connected_rooms[j].coordinates.x), 2) + pow((d->rooms[i].coordinates.y - connected_rooms[j].coordinates.y), 2));
                
                int closest_distance = sqrt(pow((d->rooms[i].coordinates.x - closest_room.coordinates.x), 2) + pow((d->rooms[i].coordinates.y - closest_room.coordinates.y), 2));

                if (distance < closest_distance)
                {
                    closest_room = connected_rooms[j];
                }
            }
        }

        /* Write corridors to dungeon */
        int x = (i % 2 == 0) ? d->rooms[i].coordinates.x : d->rooms[i].coordinates.x + d->rooms[i].width - 1; // add randomness by writing corridor from second point on even indexes 

        int y = (i % 2 == 0) ? d->rooms[i].coordinates.y : d->rooms[i].coordinates.y + d->rooms[i].height - 1;
        
        int x_closest = (i % 2 == 0) ? closest_room.coordinates.x : closest_room.coordinates.x + closest_room.width - 1;

        int y_closest = (i % 2 == 0) ? closest_room.coordinates.y : closest_room.coordinates.y + closest_room.height - 1;

        while(x != x_closest) 
        {
            int direction = (x - x_closest) / abs(x - x_closest);
            x = x - (direction * 1);

            d->map[y][x] = (d->map[y][x] == ter_floor_room) ? ter_floor_room : ter_floor_hall;
        }
                
        while(y != y_closest)
        {
            int direction = (y - y_closest) / abs(y - y_closest);
            y = y - (direction * 1);

            d->map[y][x] = (d->map[y][x] == ter_floor_room) ? ter_floor_room : ter_floor_hall;
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
int generate_staircases(dungeon_t *d)
{
    d->num_stairs_up = rand() % STAIRS_MAX + 1;
    d->stairs_up = malloc(d->num_stairs_up * sizeof(point_t));

    int i;
    for (i = 0; i < d->num_stairs_up;)
    {
        int _x = rand() % DUNGEON_X;
        int _y = rand() % DUNGEON_Y;

        terrain_t *p;
        p = &(d->map[_y][_x]);

        if (*p == ter_floor_hall || *p == ter_floor_room)
        {
            point_t coord = {_x, _y};
            d->stairs_up[i] = coord; 

            *p = ter_stairs_up;
            i++;
        }
    }

    d->num_stairs_down = rand() % STAIRS_MAX + 1;
    d->stairs_down = malloc(d->num_stairs_down * sizeof(point_t));

    int j;
    for (j = 0; j < d->num_stairs_down;)
    {
        int _x = rand() % DUNGEON_X;
        int _y = rand() % DUNGEON_Y;

        terrain_t *p;
        p = &(d->map[_y][_x]);

        if (*p == ter_floor_hall || *p == ter_floor_room)
        {
            point_t coord = {_x, _y};
            d->stairs_down[j] = coord; 

            *p = ter_stairs_down;
            j++;
        }
    }
    return 0;
}

int generate_hardness(dungeon_t *d)
{
    int i, j;
    for (i = 0; i < DUNGEON_Y; i++)
    {
        for (j = 0; j < DUNGEON_X; j++)
        {
            terrain_t t = d->map[i][j];
            if (t == ter_wall || t == ter_wall_immutable)
            {
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
int generate_terrain(dungeon_t *d)
{  
    /* Generate immutable border and walls */
    generate_border(d);

    /* Generate room terrain */
    int i, j;
    for (i = 0; i < d->num_rooms; i++)
    {
        int x, y;
        for (y = d->rooms[i].coordinates.y; y < (d->rooms[i].coordinates.y + d->rooms[i].height); y++)
        {
            for (x = d->rooms[i].coordinates.x; x < (d->rooms[i].coordinates.x + d->rooms[i].width); x++)
            {
                d->map[y][x] = ter_floor_room;
            }
        }
    }
   
    /* Generate up stairs terrain */
    for (i = 0; i < d->num_stairs_up; i++)
    {
        d->map[d->stairs_up[i].y][d->stairs_up[i].x] = ter_stairs_up;
    }

    /* Generate down stairs terrain */
    for (i = 0; i < d->num_stairs_down; i++)
    {
        d->map[d->stairs_down[i].y][d->stairs_down[i].x] = ter_stairs_down;
    }

    /* Generate corridor terrain */
    for (i = 0; i < DUNGEON_Y; i++)
    {
        for (j = 0; j < DUNGEON_X; j++)
        {
            terrain_t *t = &(d->map[i][j]);
            
            if (j == 0 || j == 79)
            {
                *t = ter_wall_immutable; 
            }

            if (i == 0 || i == 20)
            {
                *t = ter_wall_immutable; 
            }

            uint8_t h = d->hardness[i][j];

            if (h == 0 && *t == ter_wall)
            {
                *t = ter_floor_hall;
            }
        }
    }

    /* pc_coordinates */
    d->map[d->pc_coordinates.y][d->pc_coordinates.x] = ter_player;

    return 0;
}
