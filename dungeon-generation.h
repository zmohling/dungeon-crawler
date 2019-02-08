#ifndef DUNGEON_GEN
#define DUNGEON_GEN

typedef struct Room Room;

char * generate(void);
void generate_border(char *);
void generate_rooms(char *, Room *, int, int, int);
void generate_corridors(char *, Room *, int);
void generate_staircases(char *, int, int);
int intersects(Room, Room);
int out_of_bounds(Room, int, int);

#endif
