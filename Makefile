SOURCE=dungeon_crawler.c dungeon.c path_finder.c heap.c character.c move.c util.c event_simulator.c
EXE=dungeon_crawler
FLAGS=-O -Werror -lm
DEV_FLAGS=-Wall -ggdb3 -lm

all: $(SOURCE) clean
	gcc $(SOURCE) -o $(EXE) $(DEV_FLAGS)

clean:
	rm -f $(EXE) *~ vgcore.* 
