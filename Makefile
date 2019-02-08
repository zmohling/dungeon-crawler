SOURCE=main.c dungeon-generation.c dungeon-io.c
EXE=dungeon-crawler
FLAGS=-O -Werror -lm
DEV_FLAGS=-Wall -ggdb3 -lm

all: $(SOURCE) clean
	gcc $(SOURCE) -o $(EXE) $(DEV_FLAGS)

clean:
	rm -f $(EXE) *~ vgcore.* 
