#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

typedef struct Room {
    int x1, y1, x2, y2;
} Room;

/* Prototypes */
void draw_dungeon(char *, Room *);
void generate_rooms(Room *);
int intersects(Room, Room);

int main(int argc, char *argv[])
{
    char dungeon[21][80];
    Room rooms[6];

    draw_dungeon(dungeon, rooms);

    return 0;
}

void draw_dungeon(char *dungeon, Room *rooms)
{
    int i, j;
    for (i = 0; i < 21; i++)
    {
        for (j = 0; j < 80; j++)
        {
            char *p;
            p = &(dungeon + (i * 80) + j)
            
            *(dungeon + (i * 80) + j) = ' ';

            if (j == 0 || j == 79)
            {
                *(dungeon + (i * 80) + j) = '|';
            }

            if (i == 0 || i == 20)
            {
                *(dungeon + (i * 80) + j) = '-';
            }

            printf("%c", *(dungeon + (i * 80) + j));
        }

        printf("\n");
    }
}

void generate_rooms(Room *rooms)
{
    srand(time(NULL));
  
    int i;
    for (i = 0; i < 6; i++)
    {
        int _intersects = 0;
        do {

        
            rooms[i].x1 = rand() % 77 + 2;
            rooms[i].y1 = rand() % 19 + 2;
            rooms[i].x2 = rooms[i].x1 + rand() % 8 + 4;
            rooms[i].y2 = rooms[i].y1 + rand() % 6 + 3;

            int j;
            for (j = (i - 1); j >= 0; j--)
            {
               _intersects = intersects(rooms[i], rooms[j]);

               if (_intersects == 1)
               {
                   break;
               }
            }

        } while(_intersects);  
    }

}

int intersects(Room a, Room b)
{
    if (a.x1 > b.x2 || a.x2 < b.x1 || a.y1 > b.y2 || a.y2 < b.y1)
    {
        return 0;
    }
    
    return 1;
}
