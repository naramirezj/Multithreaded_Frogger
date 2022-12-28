CC := clang
CFLAGS := -g -Wall -Werror -Wno-unused-function -Wno-unused-variable

all: game

clean:
	rm -f game

game: game.c player.c player.h obstacle.c obstacle.h
	$(CC) $(CFLAGS) -o game game.c player.c obstacle.c -lncurses -lpthread -lm


