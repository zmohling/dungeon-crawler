#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

#include "character.h"
#include "dungeon.h"
#include "event_simulator.h"
#include "path_finder.h"
#include "util.h"

void render_hardness_map(dungeon_t *);
void render_movement_cost_map(dungeon_t *);
void render_distance_map(dungeon_t *);
void render_tunnel_distance_map(dungeon_t *);

int main(int argc, char *argv[]) {
    /* Check arguments */
    if (argc > 5) {
        fprintf(stderr, "Ussage: %s [--save][--load][--nummon]\n", argv[0]);
        exit(-1);
    }

    /* Random seed */
    int seed = time(NULL);
    srand(seed);

    /* Initialize path to ~/.rlg327/ */
    char *path;
    path_init(&path);

    /* Dungeon*/
    dungeon_t dungeon;

    /* Number of monsters switch */
    char *num_monsters_str = "--nummon";
    int n = 5;
    if (contains(argc, argv, num_monsters_str, &n)) {
        n = atoi(argv[n + 1]);

        if (n > MONSTERS_MAX) {
            dungeon.num_monsters = MONSTERS_MAX;
            fprintf(stderr, "Exceded maxium monster count of 10.\n");

            fprintf(stderr, "Ussage: %s [--save][--load][--nummon]\n", argv[0]);
            exit(1);
        } else if (n <= 0) {
            dungeon.num_monsters = 1;
            fprintf(stderr, "At least one monster is requred.\n");
            fprintf(stderr, "Ussage: %s [--save][--load][--nummon]\n", argv[0]);
            exit(1);
        } else {
            dungeon.num_monsters = n;
        }
    } else {
        dungeon.num_monsters = n;
    }

    /* Load switch */
    char *load_switch_str = "--load";
    if (contains(argc, argv, load_switch_str, &n)) {
        dungeon.pc = malloc(sizeof(character_t));
        read_dungeon_from_disk(&dungeon, path);
        generate_terrain(&dungeon);
    } else {
        generate_dungeon(&dungeon);
    }

    /* Save switch */
    char *save_str = "--save";
    if (contains(argc, argv, save_str, &n)) {
        write_dungeon_to_disk(&dungeon, path);
    }

    /* Start Game */
    event_simulator_start(&dungeon);

    //render_hardness_map(&dungeon);
    //render_movement_cost_map(&dungeon);
    //render_distance_map(&dungeon);
    //render_tunnel_distance_map(&dungeon);

    free(path);
    deep_free_dungeon(&dungeon);

    return 0;
}

void render_hardness_map(dungeon_t *d)
{
  /* The hardness map includes coordinates, since it's larger *
   * size makes it more difficult to index a position by eye. */
  
  point_t p;
  int i;
  
  putchar('\n');
  printf("   ");
  for (i = 0; i < DUNGEON_X; i++) {
    printf("%2d", i);
  }
  putchar('\n');
  for (p.y = 0; p.y < DUNGEON_Y; p.y++) {
    printf("%2d ", p.y);
    for (p.x = 0; p.x < DUNGEON_X; p.x++) {
      printf("%02x", d->hardness_map[p.y][p.x]);
    }
    putchar('\n');
  }
}

void render_movement_cost_map(dungeon_t *d)
{
  point_t p;

  putchar('\n');
  for (p.y = 0; p.y < DUNGEON_Y; p.y++) {
    for (p.x = 0; p.x < DUNGEON_X; p.x++) {
      if (p.x ==  d->pc->position.x &&
          p.y ==  d->pc->position.y) {
        putchar('@');
      } else {
        if (d->hardness_map[p.y][p.x] == 255) {
          printf("X");
        } else {
          printf("%d", (d->hardness_map[p.y][p.x] / 85) + 1);
        }
      }
    }
    putchar('\n');
  }
}

void render_distance_map(dungeon_t *d)
{
  point_t p;

  for (p.y = 0; p.y < DUNGEON_Y; p.y++) {
    for (p.x = 0; p.x < DUNGEON_X; p.x++) {
      if (p.x ==  d->pc->position.x &&
          p.y ==  d->pc->position.y) {
        putchar('@');
      } else {
        switch (d->map[p.y][p.x]) {
        case ter_wall:
        case ter_wall_immutable:
          putchar(' ');
          break;
        case ter_floor:
        case ter_floor_room:
        case ter_floor_hall:
        case ter_stairs:
        case ter_stairs_up:
        case ter_stairs_down:
          /* Placing X for infinity */
          if (d->non_tunnel_distance_map[p.y][p.x] == UCHAR_MAX) {
            putchar('X');
          } else {
            putchar('0' + d->non_tunnel_distance_map[p.y][p.x] % 10);
          }
          break;
        case ter_debug:
          fprintf(stderr, "Debug character at %d, %d\n", p.y, p.x);
          putchar('*');
          break;
        }
      }
    }
    putchar('\n');
  }
}

void render_tunnel_distance_map(dungeon_t *d)
{
  point_t p;

  for (p.y = 0; p.y < DUNGEON_Y; p.y++) {
    for (p.x = 0; p.x < DUNGEON_X; p.x++) {
      if (p.x ==  d->pc->position.x &&
          p.y ==  d->pc->position.y) {
        putchar('@');
      } else {
        switch (d->map[p.y][p.x]) {
        case ter_wall_immutable:
          putchar(' ');
          break;
        case ter_wall:
        case ter_floor:
        case ter_floor_room:
        case ter_floor_hall:
        case ter_stairs:
        case ter_stairs_up:
        case ter_stairs_down:
          /* Placing X for infinity */
          if (d->tunnel_distance_map[p.y][p.x] == UCHAR_MAX) {
            putchar('X');
          } else {
            putchar('0' + d->tunnel_distance_map[p.y][p.x] % 10);
          }
          break;
        case ter_debug:
          fprintf(stderr, "Debug character at %d, %d\n", p.y, p.x);
          putchar('*');
          break;
        }
      }
    }
    putchar('\n');
  }
}
