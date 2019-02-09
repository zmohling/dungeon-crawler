#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "dungeon-generation.h"
#include "dungeon-io.h"
#include "dungeon.h"

int main(int argc, char *argv[])
{
    /* Check arguments */
    if (argc < 2 || argc > 3)
    {
        fprintf(stderr, "Ussage: %s [--save][--load]\n", argv[0]);
        exit(-1);
    }

    /* Initialize path to ~/.rlg327/ */
    char *path;
    path_init(&path);

    /* Dungeon*/
    dungeon_t dungeon;

    /* Load switch */
    char *includes_str = "--load";
    if (includes(argc, argv, includes_str))
    {
        read_dungeon_from_disk(&dungeon, path);
        generate_terrain(&dungeon);
    } else {
        generate(&dungeon);
        dungeon.pc_coordinates.x = dungeon.rooms[0].coordinates.x;
        dungeon.pc_coordinates.y = dungeon.rooms[0].coordinates.y;
        dungeon.map[dungeon.pc_coordinates.y][dungeon.pc_coordinates.x] = ter_player;
    }
   
    /* Render */
    render(&dungeon);
  
    /* Save switch */
    char *save_str = "--save";
    if (includes(argc, argv, save_str))
    {
        write_dungeon_to_disk(&dungeon, path);
    }

    free(path);
    deep_free_dungeon(&dungeon);
    
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

bool includes(int argc, char *argv[], char *s)
{
    int i;
    for (i = 0; i < argc; i++)
    {
        if (!(strcmp(argv[i], s)))
        {
            return true;
        }
    }

    return false;
}

int deep_free_dungeon(dungeon_t *d)
{
    free(d->rooms);
    free(d->stairs_up);
    free(d->stairs_down);

    return 0;
}
