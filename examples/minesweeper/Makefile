ENGINE_BUILD := ../../build/linux/yoyoengine
ENGINE_MAKEFILE := ../../

CC = gcc
CFLAGS = -Wall -Wextra -I$(ENGINE_BUILD)/include
LDFLAGS = -L$(ENGINE_BUILD)/dependencies
LDLIBS = -lyoyoengine -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lfreetype -lmpg123 -lpng16 -llua -lm -ljansson

all: minesweeper

minesweeper: minesweeper.c
	$(MAKE) -C $(dir $(ENGINE_MAKEFILE)) linux
	$(CC) $(CFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@ -Wl,-rpath=$(ENGINE_BUILD)/dependencies

clean:
	rm -f minesweeper

debug_linux: CFLAGS += -DDEBUG -g
debug_linux: minesweeper

gdb: clean debug_linux
	gdb ./minesweeper

valgrind: clean debug_linux
	valgrind --leak-check=full --show-leak-kinds=all ./minesweeper

.PHONY: all clean