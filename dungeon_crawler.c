#include <stdio.h>
#include <stdlib.h>

#include "dungeon.h"
#include "path_finder.h"
#include "util.h"

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

    /* Save switch */
    char *save_str = "--save";
    if (contains(argc, argv, save_str)) {
        write_dungeon_to_disk(&dungeon, path);
    }

    free(path);
    deep_free_dungeon(&dungeon);

    return 0;
}
