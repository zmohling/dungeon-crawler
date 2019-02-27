#include <stdio.h>
#include <stdlib.h>

#include "dungeon.h"
#include "path_finder.h"
#include "util.h"
#include "character.h"

int test() {
    static int sequencer = 0;


    character_t c;
    c.is_alive = true;
    c.speed = (rand() % 16) + 5;
    c.sequence_num = sequencer++;

    c.is_pc = false;
    c.position.x = 0;
    c.position.y = 0;

    c.npc.has_seen_pc = false;
    character_assign_characteristics(&c);

    printf("%x", c.npc.characteristics & 0xff);

    return 0;
}

int main(int argc, char *argv[]) {
    /* Check arguments */
    if (argc > 3) {
        fprintf(stderr, "Ussage: %s [--save][--load]\n", argv[0]);
        exit(-1);
    }

    /* Initialize path to ~/.rlg327/ */
    char *path;
    path_init(&path);

    /* Dungeon*/
    dungeon_t dungeon;

    /* Load switch */
    char *load_switch_str = "--load";
    if (contains(argc, argv, load_switch_str)) {
        read_dungeon_from_disk(&dungeon, path);
        generate_terrain(&dungeon);
    } else {
        generate_dungeon(&dungeon);
        dungeon.pc.position.x = dungeon.rooms[0].coordinates.x;
        dungeon.pc.position.y = dungeon.rooms[0].coordinates.y;
        dungeon.map[dungeon.pc.position.y][dungeon.pc.position.x] =
            ter_player;
    }

    /* Render */
    render_dungeon(&dungeon);
    non_tunnel_distance_map(&dungeon);
    tunnel_distance_map(&dungeon);


    //test();

    /* Save switch */
    char *save_str = "--save";
    if (contains(argc, argv, save_str)) {
        write_dungeon_to_disk(&dungeon, path);
    }

    free(path);
    deep_free_dungeon(&dungeon);

    return 0;
}
