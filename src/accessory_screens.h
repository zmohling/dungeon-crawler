#ifndef ACCESSORY_SCREENS_H
#define ACCESSORY_SCREENS_H

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

class dungeon_t;

void endscreen(int);
void monster_list(dungeon_t *);

#endif
