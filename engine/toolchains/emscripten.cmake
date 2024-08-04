# This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
# Copyright (C) 2024  Ryan Zmuda
#
# Licensed under the MIT license. See LICENSE file in the project root for details.

set(CMAKE_SYSTEM_NAME Emscripten)
set(CMAKE_C_COMPILER "emcc")
set(CMAKE_CXX_COMPILER "em++")
set(CMAKE_CXX_FLAGS "--bind")
set(CMAKE_C_FLAGS "")
set(EMSCRIPTEN ON CACHE BOOL "Enable Emscripten toolchain" FORCE)

target_link_options(${GAME_NAME} PRIVATE --bind -Wbad-function-cast -Wcast-function-type -sALLOW_MEMORY_GROWTH=1 -sMAXIMUM_MEMORY=1gb --preload-file ${CMAKE_BINARY_DIR}@/)
set(CMAKE_EXECUTABLE_SUFFIX ".html")