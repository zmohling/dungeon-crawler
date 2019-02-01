#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

typedef struct Room {
    int x1, y1, x2, y2;
} Room;

/* Prototypes */
void draw_dungeon(char *, Room *, int, int);
void generate_rooms(Room *, int, int, int);
int intersects(Room, Room);
int out_of_bounds(Room, int, int);

/* Debug vars */
int attempts_to_generate = 0;

int main(int argc, char *argv[])
{
    int x = 80;
    int y = 21;
    int MAX_ROOMS = 6;

    char dungeon[y][x];
    Room rooms[MAX_ROOMS];

    /* Generate rooms */
    generate_rooms(rooms, MAX_ROOMS, x, y); 
    draw_dungeon(dungeon, rooms, x, y);

    /*
    int i;
    for (i = 0; i < MAX_ROOMS; i++)
    {
        printf("Room %d: (%d, %d) -> (%d, %d)\n", i, rooms[i].x1, rooms[i].y1, rooms[i].x2, rooms[i].y2);
    }
    */

    printf("Attempts to generate: %d\n", attempts_to_generate);
    return 0;
}

void draw_dungeon(char *dungeon, Room *rooms, int x, int y)
{
    int i, j;
    for (i = 0; i < 21; i++)
    {
        for (j = 0; j < 80; j++)
        {
            char *p;
            p = (dungeon + (i * 80) + j);
            
            *(p) = ' ';

            if (j == 0 || j == 79)
            {
                *(p) = '|';
            }

            if (i == 0 || i == 20)
            {
                *(p) = '-';
            }

            int k;
            for (k = 0; k < 6; k++)
            {
                Room room = rooms[k];
                if (j >= room.x1 && j <= room.x2 && i >= room.y1 && i <= room.y2)
                {
                    *p = '.';
                }
            }

            printf("%c", *(p));
        }

        printf("\n");
    }
}

void generate_rooms(Room *rooms, int num_rooms, int x, int y)
{
    srand(time(NULL));

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

            attempts_to_generate++;
        } while(_intersects || _out_of_bounds);  
    }

}

int out_of_bounds(Room room, int bound_x, int bound_y)
{
    if (room.x1 >= 2 && room.y1 >= 2 && room.x2 <= 77 && room.y2 <= 18)
    {
        //printf("(%d, %d) ", room.x2, room.y2);
        return 0;
    }

    return 1;
}

int intersects(Room a, Room b)
{
    if (a.x1 > b.x2 + 1 || a.x2 < b.x1 - 1 || a.y1 > b.y2 + 1 || a.y2 < b.y1 - 1)
    {
        return 0;
    }
    
    return 1;
}
