#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>

typedef struct Room {
    int x1, y1, x2, y2;
} Room;

/* Prototypes */
void draw_dungeon(char *, Room *);
void generate_rooms(Room *, int, int, int);
int intersects(Room, Room);
int out_of_bounds(Room, int, int);
void draw_corridors(char *, Room *);

/* Debug vars */

int main(int argc, char *argv[])
{
    int x = 80;
    int y = 21;
    int MAX_ROOMS = 6;

    char *dungeon = (char *) calloc(x * y, sizeof(char)); // dynamic memory allocation for dungeon array    
    Room rooms[MAX_ROOMS]; // rooms array  

    generate_rooms(rooms, MAX_ROOMS, x, y); 
    draw_corridors(dungeon, rooms);
    draw_dungeon(dungeon, rooms);


    free(dungeon); // free dungeon array

    return 0;
}

/* Draw dungeon. NOTE: Must be called after other generate
 * and draw functions.
 * char *dungeon: Dungeon array.
 * Room *rooms: Room array.
 */
void draw_dungeon(char *dungeon, Room *rooms)
{
    int i, j;
    for (i = 0; i < 21; i++)
    {
        for (j = 0; j < 80; j++)
        {
            char *p;
            p = (dungeon + (i * 80) + j);
            
            /* Set all zero'd bytes to spaces */
            if (*p == 0)
            {
                *p = ' ';
            }

            /* Set border */
            if (j == 0 || j == 79)
            {
                *p = '|';
            }

            if (i == 0 || i == 20)
            {
                *p = '-';
            }

            /* Set rooms */
            int k;
            for (k = 0; k < 6; k++)
            {
                Room room = rooms[k];
                if (j >= room.x1 && j <= room.x2 && i >= room.y1 && i <= room.y2)
                {
                    *p = '.';
                }
            }

            printf("%c", *p);
        }

        printf("\n");
    }
}

/*
 * Draw corridors between rooms. NOTE: Must be called
 * before draw_dungeon.
 * char *dungeon: Dungeon array.
 * Room *rooms: Room array.
 */
void draw_corridors(char *dungeon, Room *rooms)
{
    Room connected_rooms[5];
    connected_rooms[0] = rooms[0];

    int i;
    for (i = 1; i < 6; i++)
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

        /* Draw corridors */
        
        int x = rooms[i].x1;
        int y = rooms[i].y1;
            
        while(x != closest_room.x1)
        {
            int direction = (x - closest_room.x1) / abs(x - closest_room.x1);
            x = x - (direction * 1);

            char *p;
            p = (dungeon + (y * 80) + x);

            *p = '#';
        }
                
        while(y != closest_room.y1)
        {
            int direction = (y - closest_room.y1) / abs(y - closest_room.y1);
            y = y - (direction * 1);

            char *p;
            p = (dungeon + (y * 80) + x);

            *p = '#';
        }
        
        connected_rooms[i] = rooms[i]; 
    }
}

/*
 * Populates *rooms array with num_rooms number of rooms
 * Room *rooms: Room array to populate.
 * int num_rooms: Number of rooms to generate.
 * int x: Size of dungeon in the x direction.
 * int y: Size of dungeon in the y direction.
 * int margin: Buffer of spaces between dungeon edge and room generation.
 */
void generate_rooms(Room *rooms, int num_rooms, int x, int y)
{
    int seed = time(NULL);
    srand(seed);
    printf("Seed: %d\n", seed);

    int attempts_to_generate = 0;
    
    int i;
    for (i = 0; i < num_rooms; i++)
    {
        int _intersects = 0;
        int _out_of_bounds = 0;
        do {

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
        } while(_intersects || _out_of_bounds);  
    }

}

/* 
 * Returns 1 (true) if the specified room is out of bounds. 
 * int bound_x: Index of bound in the x direction.
 * int bound_y: Index of bound in the y direction.
 * int margin: Buffer of spaces between dungeon edge and room generation. 
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
 * Returns 1 (true) if the two specified rooms are intersecting 
 * margin: Number of spaces between rooms
 */
int intersects(Room a, Room b)
{
    if (a.x1 > b.x2 + 1 || a.x2 < b.x1 - 1 || a.y1 > b.y2 + 1 || a.y2 < b.y1 - 1)
    {
        return 0;
    }
    
    return 1;
}
