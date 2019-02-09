#include <stdlib.h>
#include <stdio.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>

#include "dungeon-generation.h"
#include "dungeon.h"

int read_dungeon_from_disk(char *path) {
    
    FILE *f;
    if ((f = fopen(path, "r")) != NULL)
    {

    }
    return 0;
}

int write_dungeon_to_disk(char *path) {
    
    FILE *f;
    if ((f = fopen(path, "w")) == NULL)
    {
        fprintf(stderr, "Error: could not save dungeon (%d)\n", errno);
        exit(-5);
    }

    char str[] = "RLG327-S2019";
    fwrite(str, sizeof(str), 1, f);

    fclose(f);

    return 0;
}

int render(dungeon_t *d)
{
    int i, j;
    for (i = 0; i < DUNGEON_Y; i++)
    {
        for (j = 0; j < DUNGEON_X; j++)
        {
            char c;

            /* Border */
            
            if (i == 0 || i == (DUNGEON_Y - 1))
            {
                c = '-';
            } else if (j == 0 || j == (DUNGEON_X - 1))
            {
                c = '|';
            }

            /* Terrain */
            terrain_t t = d->map[i][j];
            switch(t) {
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
                    c = '>';
                    break;
                case ter_stairs_down:
                    c = '<';
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

int main(int argc, char *argv[])
{
    /* Check arguments */
    if (argc < 2 || argc > 3)
    {
        fprintf(stderr, "Ussage: %s [--save][--load]\n", argv[0]);
        exit(-1);
    }

    /* FS setup */
    char *home = getenv("HOME");
    char *dir = malloc(strlen(home) + strlen("/.rlg327/") + 1);
    strcpy(dir, home);
    strcat(dir, "/.rlg327/");

    if(mkdir(dir, 0755) == -1)
    {
        /* errno == 17 is an existing directory */
        if (errno != 17)
        {
            fprintf(stderr, "Error: ~/.rlg327/ could not be found nor created. (%d)\n", errno);
            exit(-1);
        }
    }

    /* Dot file directory' path */
    char *path;
    path = malloc(strlen(home) + strlen("/.rlg327/dungeon") + 1);
    strcpy(path, home);
    strcat(path, "/.rlg327/dungeon");

    dungeon_t dungeon;
    generate(&dungeon);

    free(path);
    //free(dungeon);
   
    printf("PC Coordinates\n\tx: %d, y: %d\n", dungeon.pc_coordinates.x, dungeon.pc_coordinates.y);
    printf("Number of rooms: %d\n", dungeon.num_rooms);

    int i;
    for (i = 0; i < dungeon.num_rooms; i++)
    {
        printf("\tRoom %d:\n", i);
        printf("\t\tx: %d, y: %d\n", dungeon.rooms[i].coordinates.x, dungeon.rooms[i].coordinates.y);
        printf("\t\twidth: %d\n", dungeon.rooms[i].width);
        printf("\t\theight: %d\n", dungeon.rooms[i].height);
    }


    render(&dungeon);
    
    return 0;
}
