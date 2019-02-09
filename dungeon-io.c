#include <endian.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "dungeon-generation.h"
#include "dungeon-io.h"
#include "dungeon.h"

int path_init(char *path)
{
    /* FS setup */
    char *home = getenv("HOME");
    char *dir = malloc(strlen(home) + strlen("/.rlg327/") + 1);
    strcpy(dir, home);
    strcat(dir, "/.rlg327/");

    if(!mkdir(dir, 0755))
    {
        /* errno == 17 is an existing directory */
        if (errno != 17)
        {
            fprintf(stderr, "Error: ~/.rlg327/ could not be found nor created. (%d)\n", errno);
            exit(-1);
        }
    }

    /* Dot file directory's path */
    path = malloc(strlen(home) + strlen("/.rlg327/dungeon") + 1);
    strcpy(path, home);
    strcat(path, "/.rlg327/dungeon");
}

int read_dungeon_from_disk(dungeon_t *d, char *path) {
    
    FILE *f;
    if ((f = fopen(path, "r")) == NULL)
    {
        fprintf(stderr, "Error: could not open dungeon file. (%d)\n", errno);
        exit(-4);
    }

    char file_type_marker[12]; 
    if (!fread(file_type_marker, 12, 1, f))
    {
        fprintf(stderr, "Error: could not load <file_type_marker>. (%d)\n", errno);
        exit(-6);
    }

    uint32_t file_version;
    if (!fread(&file_version, sizeof(uint32_t), 1, f))
    {
        fprintf(stderr, "Error: could not load <file_version>. (%d)\n", errno);
        exit(-7);
    }
    file_version = be32toh(file_version);

    uint32_t file_size;
    if (!fread(&file_size, sizeof(uint32_t), 1, f))
    {
        fprintf(stderr, "Error: could not load <file_size>. (%d)\n", errno);
        exit(-8);
    }
    file_size = be32toh(file_size);

    /* pc_coordinates */
    if (!fread(&(d->pc_coordinates.x), sizeof(char), 1, f))
    {
        fprintf(stderr, "Error: could not load <pc_coordinates.x>. (%d)\n", errno);
        exit(-9);
    }
    if (!fread(&(d->pc_coordinates.y), sizeof(char), 1, f))
    {
        fprintf(stderr, "Error: could not load <pc_coordinates.y>. (%d)\n", errno);
        exit(-10);
    }

    /* hardness */
    int i, j;
    for (i = 0; i < DUNGEON_Y; i++)
    {
        for (j = 0; j < DUNGEON_X; j++)
        {
            if (!fread(&(d->hardness[i][j]), sizeof(uint8_t), 1, f))
            {
                fprintf(stderr, "Error: could not load <hardness[%d][%d]>. (%d)\n", i, j, errno);
                exit(-11);
            }
        }
    }

    /* num_rooms */
    uint16_t be_num_rooms;
    if (!fread(&be_num_rooms, sizeof(uint16_t), 1, f))
    {
        fprintf(stderr, "Error: could not load <num_rooms>. (%d)\n", errno);
        exit(-12);
    }
    d->num_rooms = be16toh(be_num_rooms);

    /* rooms array*/
    if(!(d->rooms = malloc(d->num_rooms * sizeof(room_t))))
    {
        fprintf(stderr, "Error: could not allocate memory for <*rooms>. (%d)\n", errno);
        exit(-13);
    }

    for (i = 0; i < d->num_rooms; i++)
    {
        if (!fread(&(d->rooms[i].coordinates.x), sizeof(uint8_t), 1, f))
        {
            fprintf(stderr, "Error: could not load <d->rooms[i].coordinates.x>. (%d)\n", errno);
            exit(-14);
        }

        if (!fread(&(d->rooms[i].coordinates.y), sizeof(uint8_t), 1, f))
        {
            fprintf(stderr, "Error: could not load <d->rooms[i].coordinates.y>. (%d)\n", errno);
            exit(-15);
        }
        
        if (!fread(&(d->rooms[i].width), sizeof(uint8_t), 1, f))
        {
            fprintf(stderr, "Error: could not load <d->rooms[i].width>. (%d)\n", errno);
            exit(-16);
        }
        
        if (!fread(&(d->rooms[i].height), sizeof(uint8_t), 1, f))
        {
            fprintf(stderr, "Error: could not load <d->rooms[i].height>. (%d)\n", errno);
            exit(-17);
        }
    }

    /* num_stairs_up */
    uint16_t be_num_stairs_up;
    if (!fread(&be_num_stairs_up, sizeof(uint16_t), 1, f))
    {
        fprintf(stderr, "Error: could not load <num_stairs_up>. (%d)\n", errno);
        exit(-18);
    }
    d->num_stairs_up = be16toh(be_num_stairs_up);

    /* stairs_up */
    if(!(d->stairs_up = malloc(d->num_stairs_up * sizeof(point_t))))
    {
        fprintf(stderr, "Error: could not allocate memory for <*stairs_up>. (%d)\n", errno);
        exit(-19);
    }
    
    for (i = 0; i < d->num_stairs_up; i++)
    {
        if (!fread(&(d->stairs_up[i].x), sizeof(uint8_t), 1, f))
        {
            fprintf(stderr, "Error: could not load <d->stairs_up[i].x>. (%d)\n", errno);
            exit(-20);
        }

        if (!fread(&(d->stairs_up[i].y), sizeof(uint8_t), 1, f))
        {
            fprintf(stderr, "Error: could not load <d->stairs_up[i].y>. (%d)\n", errno);
            exit(-21);
        }
    }

    /* num_stairs_down */
    uint16_t be_num_stairs_down;
    if (!fread(&be_num_stairs_down, sizeof(uint16_t), 1, f))
    {
        fprintf(stderr, "Error: could not load <num_stairs_down>. (%d)\n", errno);
        exit(-22);
    }
    d->num_stairs_down = be16toh(be_num_stairs_down);

    /* stairs_up */
    if(!(d->stairs_down = malloc(d->num_stairs_down * sizeof(point_t))))
    {
        fprintf(stderr, "Error: could not allocate memory for <*stairs_down>. (%d)\n", errno);
        exit(-23);
    }
    
    for (i = 0; i < d->num_stairs_down; i++)
    {
        if (!fread(&(d->stairs_down[i].x), sizeof(uint8_t), 1, f))
        {
            fprintf(stderr, "Error: could not load <d->stairs_down[i].x>. (%d)\n", errno);
            exit(-24);
        }

        if (!fread(&(d->stairs_down[i].y), sizeof(uint8_t), 1, f))
        {
            fprintf(stderr, "Error: could not load <d->stairs_down[i].y>. (%d)\n", errno);
            exit(-25);
        }
    }
   
    fclose(f);

    return 0;
}

int write_dungeon_to_disk(dungeon_t *d, char *path) {

    FILE *f;
    if ((f = fopen(path, "w")) == NULL)
    {
        fprintf(stderr, "Error: could not open dungeon file. (%d)\n", errno);
        exit(-34);
    }

    char file_type_marker[12] = {'R', 'L', 'G', '3', '2', '7', '-', 'S', '2', '0', '1', '9'}; 
    if (!fwrite(file_type_marker, sizeof(file_type_marker), 1, f))
    {
        fprintf(stderr, "Error: could not save <file_type_marker>. (%d)\n", errno);
        exit(-36);
    }

    uint32_t temp = 0;
    uint32_t file_version = htobe32(temp);
    if (!fwrite(&file_version, sizeof(uint32_t), 1, f))
    {
        fprintf(stderr, "Error: could not save <file_version>. (%d)\n", errno);
        exit(-37);
    }

    temp = (
            sizeof(file_type_marker) + 
            sizeof(uint32_t) * 2 + 
            sizeof(uint8_t) * 2 + 
            sizeof(uint8_t) * (DUNGEON_Y * DUNGEON_X) + 
            sizeof(uint16_t) * 3 + 
            sizeof(uint8_t) * d->num_rooms + 
            sizeof(uint8_t) * d->num_stairs_up + 
            sizeof(uint8_t) * d->num_stairs_down
    );

    uint32_t file_size = htobe32(temp);
    if (!fwrite(&file_size, sizeof(uint32_t), 1, f))
    {
        fprintf(stderr, "Error: could not save <file_size>. (%d)\n", errno);
        exit(-38);
    }

    /* pc_coordinates */
    if (!fwrite(&(d->pc_coordinates.x), sizeof(char), 1, f))
    {
        fprintf(stderr, "Error: could not save <pc_coordinates.x>. (%d)\n", errno);
        exit(-39);
    }
    if (!fwrite(&(d->pc_coordinates.y), sizeof(char), 1, f))
    {
        fprintf(stderr, "Error: could not save <pc_coordinates.y>. (%d)\n", errno);
        exit(-40);
    }

    /* hardness */
    int i, j;
    for (i = 0; i < DUNGEON_Y; i++)
    {
        for (j = 0; j < DUNGEON_X; j++)
        {
            if (!fwrite(&(d->hardness[i][j]), sizeof(uint8_t), 1, f))
            {
                fprintf(stderr, "Error: could not save <hardness[%d][%d]>. (%d)\n", i, j, errno);
                exit(-41);
            }
        }
    }

    /* num_rooms */
    uint16_t num_rooms = d->num_rooms;
    uint16_t be_num_rooms = htobe16(num_rooms);
    if (!fwrite(&be_num_rooms, sizeof(uint16_t), 1, f))
    {
        fprintf(stderr, "Error: could not save <num_rooms>. (%d)\n", errno);
        exit(-42);
    }

    /* rooms array*/
    for (i = 0; i < d->num_rooms; i++)
    {
        if (!fwrite(&(d->rooms[i].coordinates.x), sizeof(uint8_t), 1, f))
        {
            fprintf(stderr, "Error: could not save <d->rooms[i].coordinates.x>. (%d)\n", errno);
            exit(-44);
        }

        if (!fwrite(&(d->rooms[i].coordinates.y), sizeof(uint8_t), 1, f))
        {
            fprintf(stderr, "Error: could not save <d->rooms[i].coordinates.y>. (%d)\n", errno);
            exit(-45);
        }
        
        if (!fwrite(&(d->rooms[i].width), sizeof(uint8_t), 1, f))
        {
            fprintf(stderr, "Error: could not save <d->rooms[i].width>. (%d)\n", errno);
            exit(-46);
        }
        
        if (!fwrite(&(d->rooms[i].height), sizeof(uint8_t), 1, f))
        {
            fprintf(stderr, "Error: could not save <d->rooms[i].height>. (%d)\n", errno);
            exit(-47);
        }
    }

    /* num_stairs_up */
    uint16_t num_stairs_up = d->num_stairs_up;
    uint16_t be_num_stairs_up = htobe16(num_stairs_up);
    if (!fwrite(&be_num_stairs_up, sizeof(uint16_t), 1, f))
    {
        fprintf(stderr, "Error: could not save <num_stairs_up>. (%d)\n", errno);
        exit(-48);
    }

    /* stairs_up */
    for (i = 0; i < d->num_stairs_up; i++)
    {
        if (!fwrite(&(d->stairs_up[i].x), sizeof(uint8_t), 1, f))
        {
            fprintf(stderr, "Error: could not save <d->stairs_up[i].x>. (%d)\n", errno);
            exit(-50);
        }

        if (!fwrite(&(d->stairs_up[i].y), sizeof(uint8_t), 1, f))
        {
            fprintf(stderr, "Error: could not save <d->stairs_up[i].y>. (%d)\n", errno);
            exit(-51);
        }
    }

    /* num_stairs_down */
    uint16_t num_stairs_down = d->num_stairs_down;
    uint16_t be_num_stairs_down = htobe16(num_stairs_down);
    if (!fwrite(&be_num_stairs_down, sizeof(uint16_t), 1, f))
    {
        fprintf(stderr, "Error: could not save <num_stairs_down>. (%d)\n", errno);
        exit(-52);
    }

    /* stairs_up */
    for (i = 0; i < d->num_stairs_down; i++)
    {
        if (!fwrite(&(d->stairs_down[i].x), sizeof(uint8_t), 1, f))
        {
            fprintf(stderr, "Error: could not save <d->stairs_down[i].x>. (%d)\n", errno);
            exit(-54);
        }

        if (!fwrite(&(d->stairs_down[i].y), sizeof(uint8_t), 1, f))
        {
            fprintf(stderr, "Error: could not save <d->stairs_down[i].y>. (%d)\n", errno);
            exit(-55);
        }
    }

    fclose(f);

    return 0;
}
