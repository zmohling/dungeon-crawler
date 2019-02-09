#include <stdlib.h>
#include <stdio.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>

#include "dungeon-generation.h"

char * read_dungeon_from_disk(char *path) {
    
    FILE *f;
    if ((f = fopen(path, "r")) != NULL)
    {

    }
}

char * write_dungeon_to_disk(char *path) {
    
    FILE *f;
    if ((f = fopen(path, "w")) == NULL)
    {
        fprintf(stderr, "Error: could not save dungeon (%d)\n", errno);
        exit(-5);
    }

    char str[] = "RLG327-S2019";
    fwrite(str, sizeof(str), 1, f);

    fclose(f);

    return generate();
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

    char *dungeon;
    dungeon = write_dungeon_to_disk(path);

    free(path);
    free(dungeon);
    
    return 0;
}
