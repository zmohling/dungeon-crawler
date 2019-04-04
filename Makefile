all: src/parser/testdir/lex.yy.c
	gcc src/parser/testdir/lex.yy.c -lfl -o dungeon_crawler

lex.yy.c: src/parser/testdir/test.l
	lex src/parser/testdir/test.l

clean:
	rm dungeon_crawler lex.yy.c
