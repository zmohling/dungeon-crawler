#ifndef INPUT_H
#define INPUT_H

typedef enum __attribute__((__packed__)) pc_movement_type {
    pc_rest,
    pc_up,
    pc_up_left,
    pc_up_right,
    pc_left,
    pc_right,
    pc_down,
    pc_down_left,
    pc_down_right,
} pc_movement_t;

typedef struct dungeon dungeon_t;

void handle_key(dungeon_t *, int);

#endif
