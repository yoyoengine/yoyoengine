# CC := gcc -v
CC := gcc
CFLAGS := -Wall -Wextra
LDFLAGS := -shared
SRC_DIR := src
BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj

# Determine PLATFORM based on target
ifeq ($(filter linux,$(MAKECMDGOALS)),linux)
    PLATFORM := linux
else ifeq ($(filter windows,$(MAKECMDGOALS)),windows)
    PLATFORM := windows
else ifeq ($(filter osx-arm,$(MAKECMDGOALS)),osx-arm)
    PLATFORM := osx-arm
endif

# use our local copies of our libs to compile (TODO: fix for macos arm)
CFLAGS += -I./dist/include
LDFLAGS += -L./dist/$(PLATFORM)
LDLIBS += -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lfreetype -lmpg123 -lpng16 -llua -lm -ljansson

# Enable debugging flags if the DEBUG variable is set
ifdef DEBUG
    CFLAGS += -DDEBUG -g
endif

BIN_DIR := $(BUILD_DIR)/$(PLATFORM)/yoyoengine
DIST_FILES := $(SRC_DIR)/dist/$(PLATFORM)

SOURCES := $(wildcard $(SRC_DIR)/*.c)
OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))

ENGINE_NAME := libyoyoengine

SHARED_LIB_DIR := $(SRC_DIR)/dist/$(PLATFORM)

SHARED_LIB_EXTENSIONS := .so .dll

define find_shared_libs
$(wildcard $(1)*$(2))
endef

SHARED_LIBS := $(foreach ext,$(SHARED_LIB_EXTENSIONS),$(call find_shared_libs,$(SHARED_LIB_DIR),$(ext)))

.PHONY: all clean linux windows copy_headers copy_libs help

all: help

linux: $(BIN_DIR)/dependencies/$(ENGINE_NAME).so copy_headers copy_libs engine_resources

windows: $(BIN_DIR)/dependencies/$(ENGINE_NAME).dll copy_headers copy_libs engine_resources

osx-arm: $(BIN_DIR)/dependencies/$(ENGINE_NAME).so copy_headers copy_libs engine_resources

############################ ACTUAL ENGINE LIB OUTPUT ###############################

# LINUX  & OSX
$(BIN_DIR)/dependencies/$(ENGINE_NAME).so: $(OBJECTS) $(SHARED_LIBS)
	@mkdir -p $(BIN_DIR)/dependencies
	$(CC) $(LDFLAGS) -o $@ $^

# WINDOWS
$(BIN_DIR)/dependencies/$(ENGINE_NAME).dll: $(OBJECTS) $(SHARED_LIBS)
	@mkdir -p $(BIN_DIR)/dependencies
	$(CC) $(LDFLAGS) -o $(BIN_DIR)/dependencies/$(ENGINE_NAME).dll $^

############################ COPY PLATFORM DIST FILES ###############################

dist_files: $(DIST_FILES)
	@mkdir -p $(BIN_DIR)/dependencies
	cp -r $(SRC_DIR)/dist/$(PLATFORM)/* $(BIN_DIR)/dependencies

engine_resources: $(DIST_FILES)
	@mkdir -p $(BIN_DIR)/engine_resources
	cp -r $(SRC_DIR)/dist/assets/* $(BIN_DIR)/engine_resources

############################ BUILD OBJECT FILES ###############################

$(OBJ_DIR)/%.o: $(ENGINE_SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -fPIC -c -o $@ $< -Wl,-rpath=$(SRC_DIR)/dist/$(PLATFORM)

# Copy header files to the lib folder within the build directory
copy_headers: $(DIST_FILES)
	@mkdir -p $(BIN_DIR)/include
	cp -r $(SRC_DIR)/dist/include* $(BIN_DIR)

# Copy external libraries to the dependencies folder within the build directory
copy_libs: $(BIN_DIR)/dependencies $(DIST_FILES)
	@mkdir -p $(BIN_DIR)/dependencies
	cp $(SRC_DIR)/dist/$(PLATFORM)/* $(BIN_DIR)/dependencies

# Help target
help:
	@echo "+--------------------------------------------------------------------+"
	@echo "| Yoyo Engine - Ryan Zmuda 2023                                      |"
	@echo "+--------------------------------------------------------------------+"
	@echo "| Available targets:                                                 |"
	@echo "| - linux         : Build the Linux version                          |"
	@echo "| - windows       : Build the Windows version                        |"
	@echo "| - copy_headers  : Copy header files to lib folder                  |"
	@echo "| - copy_libs     : Copy external libraries to dependencies folder   |"
	@echo "| - clean         : Clean up build artifacts                         |"
	@echo "| - cloc          : Count lines of code                              |"
	@echo "+--------------------------------------------------------------------+"
	@echo "| passing 'DEBUG=1' will enable debug builds                         |"
	@echo "+--------------------------------------------------------------------+"

# Clean target
clean:
	rm -rf $(BUILD_DIR)

cloc: 
	cloc ./src --exclude-dir=dist
# cloc ./src/dist/include/yoyoengine --exclude-list-file=uthash.h