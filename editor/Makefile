ENGINE_BUILD := ../../build/linux/yoyoengine
ENGINE_MAKEFILE := ../../

CC = gcc
CFLAGS = -Wall -Wextra -I$(ENGINE_BUILD)/include
LDFLAGS = -L$(ENGINE_BUILD)/dependencies
LDLIBS = -lyoyoengine -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lfreetype -lmpg123 -lpng16 -llua -lm -ljansson

all: engine editor

engine:
	$(MAKE) -C $(dir $(ENGINE_MAKEFILE)) linux

editor: editor.c
	mkdir -p ./build/linux
	$(CC) $(CFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o ./build/linux/$@ -Wl,-rpath=$(ENGINE_BUILD)/dependencies

clean:
	rm -f ./build/linux/editor

debug_linux: CFLAGS += -DDEBUG -g
debug_linux: editor

gdb: clean debug_linux
	gdb ./build/linux/editor

valgrind: clean debug_linux
	valgrind --leak-check=full --show-leak-kinds=all ./build/linux/editor

.PHONY: all clean