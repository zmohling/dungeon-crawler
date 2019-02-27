#ifndef EVENT_SIMULATOR_H
#define EVENT_SIMULATOR_H

typedef struct dungeon dungeon_t;

int event_start(dungeon_t *);
int event_game_loop(dungeon_t *);

#endif
