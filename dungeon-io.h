#ifndef DUNGEON_IO_H
# define DUNGEON_IO_H

#include "dungeon.h"

int path_init(char *);
int read_dungeon_from_disk(dungeon_t *, char *);
int write_dungeon_to_disk(dungeon_t *, char *);

#endif
