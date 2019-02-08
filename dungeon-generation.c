#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "dungeon-generation.h"

typedef struct Room {
    int x1, y1, x2, y2;
} Room;

/* Prototypes */
char * generate(void);
void generate_border(char *);
void generate_rooms(char *, Room *, int, int, int);
void generate_corridors(char *, Room *, int);
void generate_staircases(char *, int, int);
int intersects(Room, Room);
int out_of_bounds(Room, int, int);
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
char * generate(void)
{
    int seed = time(NULL);
    srand(seed);
    //printf("Seed: %d\n", seed);
    
    int x = 80;
    int y = 21;
    int NUM_ROOMS = 6 + (rand() % 2); // between 6 and 7 rooms
    Room rooms[NUM_ROOMS]; // rooms array  

    static char *dungeon;
    dungeon = (char *) calloc(x * y, sizeof(char));
    //char dungeon[y][x];
    //memset(dungeon, ' ', y * x * sizeof(char));

    generate_border(dungeon);
    generate_rooms(dungeon, rooms, NUM_ROOMS, x, y);
    generate_corridors(dungeon, rooms, NUM_ROOMS);
    generate_staircases(dungeon, x, y);

    /* Print to console */
    int i, j;
    for (i = 0; i < y; i++)
    {
        for (j = 0; j < x; j++)
        {
            printf("%c", *(dungeon + (i * x) + j));
        }

        printf("\n");
    }
   
    return dungeon;
    //free(dungeon);
}

/*
 * Generate border around edges and initialize 
 * dungeon array to spaces.
 * char *dungeon: Dungeon array to write to.
 */
void generate_border(char *dungeon)
{
    int i, j;
    for (i = 0; i < 21; i++)
    {
        for (j = 0; j < 80; j++)
        {
            char *p;
            p = (dungeon + (i * 80) + j);
            
            *p = ' '; // set everything to spaces

            if (j == 0 || j == 79)
            {
                *p = '|';
            }

            if (i == 0 || i == 20)
            {
                *p = '-';
            }
        }
    }
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
void generate_rooms(char *dungeon, Room *rooms, int num_rooms, int x, int y)
{

    int attempts_to_generate = 0;
    
    int i;
    for (i = 0; i < num_rooms; i++)
    {
        int _intersects = 0;
        int _out_of_bounds = 0;
        do {

            /* Generate random room within bounds and margin of 2 */
            rooms[i].x1 = rand() % (x - 4) + 2;
            rooms[i].y1 = rand() % (y - 4) + 2;
            rooms[i].x2 = rooms[i].x1 + rand() % 12 + 4;
            rooms[i].y2 = rooms[i].y1 + rand() % 9 + 3;

            int j;
            for (j = (i - 1); j >= 0; j--)
            {
               _intersects = intersects(rooms[i], rooms[j]);

               if (_intersects == 1)
               {
                   break;
               }
            }

            _out_of_bounds = out_of_bounds(rooms[i], (x - 1), (y - 1));

            if (attempts_to_generate > 2000)
            {
                fprintf(stderr, "Error: Could not generate rooms.");
                exit(-1);
            }
            attempts_to_generate++;
        } while(_intersects || _out_of_bounds); // continue generating until the random room is in bounds not non-intersecting.

        /* Write generated room to dungeon array */
        int x, y;
        for (y = rooms[i].y1; y <= rooms[i].y2; y++)
        {
            for (x = rooms[i].x1; x <= rooms[i].x2; x++)
            {
                char *p;
                p = (dungeon + (y * 80) + x);

                *p = '.';
            }
        }
    }
}

/* 
 * Returns 1 (true) if the specified room is out of bounds. 
 * Room room: Room which to check if in bounds.
 * int bound_x: Index of bound in the x direction.
 * int bound_y: Index of bound in the y direction.
 */
int out_of_bounds(Room room, int bound_x, int bound_y)
{
    if (room.x1 >= 2 && room.y1 >= 2 && room.x2 <= 77 && room.y2 <= 18)
    {
        return 0;
    }

    return 1;
}

/*
 * Returns 1 (true) if the two specified rooms are intersecting. 
 * Room a: First room of the pair.
 * Room b: Second room of the pair.
 */
int intersects(Room a, Room b)
{
    if (a.x1 > b.x2 + 1 || a.x2 < b.x1 - 1 || a.y1 > b.y2 + 1 || a.y2 < b.y1 - 1)
    {
        return 0;
    }
    
    return 1;
}

/*
 * Generate corridors between rooms. 
 * char *dungeon: Dungeon array to write to.
 * Room *rooms: Room array.
 * int num_rooms: Number of rooms in *rooms
 */
void generate_corridors(char *dungeon, Room *rooms, int num_rooms)
{
    Room connected_rooms[num_rooms];
    connected_rooms[0] = rooms[0];

    int i;
    for (i = 1; i < num_rooms; i++)
    {
        Room closest_room; 

        /* Find the closest room using Euclidean distance formula */
        int j;
        for (j = (i - 1); j >= 0; j--)
        {
            if (j == (i - 1))
            {
                closest_room = connected_rooms[j];
            } else {
                int distance = sqrt(pow((rooms[i].x1 - connected_rooms[j].x1), 2) + pow((rooms[i].y1 - connected_rooms[j].y1), 2));
                int closest_distance = sqrt(pow((rooms[i].x1 - closest_room.x1), 2) + pow((rooms[i].y1 - closest_room.y1), 2));
                
                if (distance < closest_distance)
                {
                    closest_room = connected_rooms[j];
                }
            }
        }

        /* Write corridors to dungeon */
        int x = (i % 2 == 0) ? rooms[i].x1 : rooms[i].x2; // add randomness by writing corridor from second point on even indexes 
        int y = (i % 2 == 0) ? rooms[i].y1 : rooms[i].y2;  
        int x_closest = (i % 2 == 0) ? closest_room.x1 : closest_room.x2;
        int y_closest = (i % 2 == 0) ? closest_room.y1 : closest_room.y2;

        while(x != x_closest) 
        {
            int direction = (x - x_closest) / abs(x - x_closest);
            x = x - (direction * 1);

            char *p;
            p = (dungeon + (y * 80) + x);

            *p = (*p == '.') ? '.' : '#';
        }
                
        while(y != y_closest)
        {
            int direction = (y - y_closest) / abs(y - y_closest);
            y = y - (direction * 1);

            char *p;
            p = (dungeon + (y * 80) + x);

            *p = (*p == '.') ? '.' : '#';
        }
        
        connected_rooms[i] = rooms[i]; 
    }
}

/*
 * Generate 1-2 up and down staircases in
 * rooms or corridors.
 * char *dungeon: Dungeon array to write to.
 * int x: Size of dungeon in x direction.
 * int y: Size of dungeon in y direction.
 */
void generate_staircases(char *dungeon, int x, int y)
{
    int STAIR_MAX = 3;

    int i;
    for (i = 0; i < (rand() % STAIR_MAX + 1);)
    {
        int _x = rand() % x;
        int _y = rand() % y;

        char *p;
        p = (dungeon + (_y * 80) + _x);

        if (*p == '#' || *p == '.')
        {
            *p = '>';
            i++;
        }
    }

    int j;
    for (j = 0; j < (rand() % STAIR_MAX + 1);)
    {
        int _x = rand() % x;
        int _y = rand() % y;

        char *p;
        p = (dungeon + (_y * 80) + _x);

        if (*p == '#' || *p == '.')
        {
            *p = '<';
            j++;
        }
    }
}
