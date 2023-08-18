CC := gcc
CFLAGS := -Wall -Wextra -Ilib
LDFLAGS := -shared
SRC_DIR := src
BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj
BIN_DIR_LINUX := $(BUILD_DIR)/linux/yoyoengine
BIN_DIR_WINDOWS := $(BUILD_DIR)/windows/yoyoengine

SOURCES := $(wildcard $(SRC_DIR)/*.c)
OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))

ENGINE_NAME := libyoyoengine

SHARED_LIB_DIR_LINUX := $(SRC_DIR)/dist/linux
SHARED_LIB_DIR_WINDOWS := $(SRC_DIR)/dist/windows

SHARED_LIB_EXTENSIONS := .so .dll

define find_shared_libs
$(wildcard $(1)*$(2))
endef

SHARED_LIBS_LINUX := $(foreach ext,$(SHARED_LIB_EXTENSIONS),$(call find_shared_libs,$(SHARED_LIB_DIR_LINUX),$(ext)))
SHARED_LIBS_WINDOWS := $(foreach ext,$(SHARED_LIB_EXTENSIONS),$(call find_shared_libs,$(SHARED_LIB_DIR_WINDOWS),$(ext)))

DIST_FILES_LINUX := $(wildcard dist/linux/*)
DIST_FILES_WINDOWS := $(wildcard dist/windows/*)

.PHONY: all clean linux windows copy_headers copy_libs help

all: help

linux: $(BIN_DIR_LINUX)/$(ENGINE_NAME).so $(BIN_DIR_LINUX)/copy_headers $(BIN_DIR_LINUX)/copy_libs $(BIN_DIR_LINUX)/engine_resources

windows: $(BIN_DIR_WINDOWS)/$(ENGINE_NAME).dll $(BIN_DIR_WINDOWS)/dist_files $(BIN_DIR_WINDOWS)/engine_resources

$(BIN_DIR_LINUX)/$(ENGINE_NAME).so: $(OBJECTS) $(SHARED_LIBS_LINUX)
	@mkdir -p $(BIN_DIR_LINUX)
	$(CC) $(LDFLAGS) -o $@ $^

$(BIN_DIR_WINDOWS)/$(ENGINE_NAME).dll: $(OBJECTS) $(SHARED_LIBS_WINDOWS)
	@mkdir -p $(BIN_DIR_WINDOWS)
	$(CC) $(LDFLAGS) -o $@ $^

$(BIN_DIR_LINUX)/dist_files: $(DIST_FILES_LINUX)
	@mkdir -p $(BIN_DIR_LINUX)/dependencies
	cp -r $(SRC_DIR)/dist/linux/* $(BIN_DIR_LINUX)/dependencies

$(BIN_DIR_WINDOWS)/dist_files: $(DIST_FILES_WINDOWS)
	@mkdir -p $(BIN_DIR_WINDOWS)/dependencies
	cp -r $(SRC_DIR)/dist/windows/* $(BIN_DIR_WINDOWS)/dependencies

$(BIN_DIR_LINUX)/engine_resources: $(DIST_FILES_LINUX)
	@mkdir -p $(BIN_DIR_LINUX)/engine_resources
	cp -r $(SRC_DIR)/dist/assets/* $(BIN_DIR_LINUX)/engine_resources

$(BIN_DIR_WINDOWS)/engine_resources: $(DIST_FILES_WINDOWS)
	@mkdir -p $(BIN_DIR_WINDOWS)/engine_resources
	cp -r $(SRC_DIR)/dist/assets/* $(BIN_DIR_WINDOWS)/engine_resources

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -fPIC -c -o $@ $<

# Copy header files to the lib folder within the build directory
$(BIN_DIR_LINUX)/copy_headers: $(DIST_FILES_LINUX)
	@mkdir -p $(BIN_DIR_LINUX)/lib
	cp -r $(SRC_DIR)/lib/* $(BIN_DIR_LINUX)/lib

# Copy external libraries to the dependencies folder within the build directory
$(BIN_DIR_LINUX)/copy_libs: $(DIST_FILES_LINUX)
	@mkdir -p $(BIN_DIR_LINUX)/dependencies
	cp -r $(SRC_DIR)/dist/linux/* $(BIN_DIR_LINUX)/dependencies

# Help target
help:
	@echo "Available targets:"
	@echo "  linux         : Build the Linux version"
	@echo "  windows       : Build the Windows version"
	@echo "  copy_headers  : Copy header files to lib folder"
	@echo "  copy_libs     : Copy external libraries to dependencies folder"
	@echo "  clean         : Clean up build artifacts"

# Clean target
clean:
	rm -rf $(BUILD_DIR)
