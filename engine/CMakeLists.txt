# This file is a part of yoyoengine. (https://github.com/yoyolick/yoyoengine)
# Copyright (C) 2024  Ryan Zmuda

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

######################################################################################
#                                      PREAMBLE                                      #
######################################################################################
# You would never believe how much time I've spent on this file. I hate cmake more   #
# then I think I've ever hated anything in my entire life. It was supposed to be an  #
# easy, simple, and elegant to build and link things together. It was supposed to be.#
# This file might as well be held together by 1 ply toilet paper. If you are even    #
# attempting to read this, or god forbid, are me in the future come back to wrestle  #
# yet again with this god forsaken file, I am so extremely sorry. Good luck.         #
######################################################################################

###############
#    setup    #
###############

cmake_minimum_required(VERSION 3.22.1)
project(yoyoengine)

file(GLOB_RECURSE SOURCES CMAKE_CONFIGURE_DEPENDS src/*.c)

add_library(yoyoengine SHARED ${SOURCES})

target_include_directories(yoyoengine PRIVATE dist/include/)

include(FetchContent)
Set(FETCHCONTENT_QUIET FALSE)

# dont build tests
set(JANSSON_WITHOUT_TESTS ON CACHE BOOL "Disable building of tests")

# if not in release mode, build in debug mode which sets cflags for max warnings and debug sigs
option(YOYO_ENGINE_BUILD_RELEASE "Build the engine in release mode" ON)
if(NOT YOYO_ENGINE_BUILD_RELEASE)
    target_compile_options(yoyoengine PRIVATE -Wall -Wextra -g)
    message(STATUS "Building yoyoengine in debug mode")
else()
    # target_compile_options(yoyoengine PRIVATE -03) COMPILER OPTIMIZATION DESTROYS ANIMATION SYSTEM
    target_link_options(yoyoengine PRIVATE -s)
    message(STATUS "Building yoyoengine in release mode")
endif()

###############
#   jansson   #
###############

# Build as shared and without docs - NOTE: jansson doesnt give af about the shared param and builds static anyways
set(BUILD_SHARED 1 CACHE INTERNAL "")
set(JANSSON_BUILD_DOCS OFF CACHE INTERNAL "")
FetchContent_Declare(
    jansson
    GIT_REPOSITORY https://github.com/akheron/jansson.git
    GIT_TAG        v2.14
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(jansson)
target_include_directories(yoyoengine PRIVATE ${jansson_BINARY_DIR}/include)

###############
#    SDL2     #
###############

set(SDL2_DISABLE_INSTALL OFF CACHE BOOL "Disable installation of SDL2") # i forget why this is necessary but im assuming it wont work without
set(SDL_STATIC ON CACHE BOOL "Build a static library")
set(SDL_SHARED OFF CACHE BOOL "Build a shared library")
set(SDL_TEST OFF CACHE BOOL "Build the test programs")
set(SDL_STATIC_PIC ON CACHE BOOL "Build static libraries with PIC (Position Independent Code)")
FetchContent_Declare(
    SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG        release-2.28.5
    GIT_PROGRESS TRUE
)
# set(SDL_SHARED OFF CACHE BOOL "Build a shared library")
FetchContent_MakeAvailable(SDL2)
target_include_directories(yoyoengine PRIVATE ${SDL2_SOURCE_DIR}/include)

###############
#    zlib     #
###############

set(ZLIB_USE_STATIC_LIBS ON CACHE BOOL "Use static libraries for zlib")
set(ZLIB_USE_SHARED_LIBS OFF CACHE BOOL "Use shared libraries for zlib")

FetchContent_Declare(
    zlib
    GIT_REPOSITORY https://github.com/madler/zlib.git
    GIT_TAG        v1.3
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(zlib)
target_include_directories(yoyoengine PRIVATE ${zlib_SOURCE_DIR})

# needed for zlib static
set_property(TARGET zlibstatic PROPERTY POSITION_INDEPENDENT_CODE ON)

# Export zlib targets for SDL_ttf install
install(TARGETS zlibstatic
        EXPORT zlib-targets
        ARCHIVE DESTINATION lib
        INCLUDES DESTINATION include
)

# Export zlib includes for SDL_ttf install
install(DIRECTORY ${zlib_SOURCE_DIR}/
        DESTINATION include
        FILES_MATCHING PATTERN "*.h*"
)

# Export zlib config for SDL_ttf install
install(EXPORT zlib-targets
        FILE zlib-targets.cmake
        NAMESPACE zlib::
        DESTINATION lib/cmake/zlib
)

# we need to override some SDL_ttf vars to use our own impl of zlib for its freetype gzip
set(ZLIB_FOUND TRUE CACHE BOOL "ZLIB found")
list(APPEND ZLIB_INCLUDE_DIR ${zlib_SOURCE_DIR} ${zlib_BINARY_DIR})
set(ZLIB_INCLUDE_DIR ${ZLIB_INCLUDE_DIR} CACHE INTERNAL "" FORCE)
set(ZLIB_LIBRARY zlibstatic CACHE INTERNAL "" FORCE)
set(ZLIB_VERSION_STRING 1.2.11 CACHE INTERNAL "" FORCE)

###############
#  SD2L_ttf   #
###############

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build the library as a shared library")
# set(SDL2TTF_BUILD_SHARED_LIBS OFF CACHE BOOL "Build the library as a shared library")
set(SDL2TTF_SAMPLES OFF CACHE BOOL "Build the SDL2_ttf sample program(s)")
# let it know it needs to get its vendored deps
set(SDL2TTF_VENDORED ON CACHE BOOL "Use the vendored version of FreeType and HarfBuzz")

# force requiring zlib and override anything else that might try to change that
set(FT_REQUIRE_ZLIB TRUE CACHE INTERNAL "" FORCE)
set(FT_DISABLE_ZLIB FALSE CACHE INTERNAL "" FORCE)

# set(SDL2TTF_INSTALL OFF CACHE BOOL "Generate installation target")

FetchContent_Declare(
    SDL2_ttf
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf
    GIT_TAG        release-2.20.2
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(SDL2_ttf)
target_include_directories(yoyoengine PRIVATE ${SDL2_ttf_SOURCE_DIR}) # include is in the root of the repo

###############
#  SDL_image  #
###############

set(SDL2IMAGE_DEPS_SHARED OFF CACHE BOOL "Build the dependencies as shared libraries")

set(PNG_SHARED OFF CACHE BOOL "Build a shared version of the library")

set(PNG_BUILD_ZLIB ON CACHE BOOL "Build zlib")

# only allow bmp, gif, png, jpg
set(SDL2IMAGE_XV OFF CACHE BOOL "Enable Xv support")
set(SDL2IMAGE_XPM OFF CACHE BOOL "Enable XPM support")
set(SDL2IMAGE_XCF OFF CACHE BOOL "Enable XCF support")
set(SDL2IMAGE_TGA OFF CACHE BOOL "Enable TGA support")
set(SDL2IMAGE_QOI OFF CACHE BOOL "Enable QOI support")
set(SDL2IMAGE_PNM OFF CACHE BOOL "Enable PNM support")
set(SDL2IMAGE_PCX OFF CACHE BOOL "Enable PCX support")
set(SDL2IMAGE_LBM OFF CACHE BOOL "Enable LBM support")

FetchContent_Declare(
    SDL2_image
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
    GIT_TAG        release-2.8.2
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(SDL2_image)
target_include_directories(yoyoengine PRIVATE ${SDL2_image_SOURCE_DIR}/include)

# include the zlib that SDL_image uses - this is for a stray header we are missing
target_include_directories(yoyoengine PRIVATE ${SDL2_image_SOURCE_DIR}/external/zlib)

###############
#  SDL_mixer  #
###############

# disable annoying codecs we dont need that require dependencies i dont have
set(SDL2MIXER_OPUS OFF CACHE BOOL "Enable Opus support")
set(SDL2MIXER_FLAC OFF CACHE BOOL "Enable FLAC support")
set(SDL2MIXER_MOD OFF CACHE BOOL "Enable Mod support")
set(SDL2MIXER_MIDI OFF CACHE BOOL "Enable MIDI support")

# mp3 enabled
set(SDL2MIXER_MP3 ON CACHE BOOL "Enable MP3 support")

# wav enabled
set(SDL2MIXER_WAV ON CACHE BOOL "Enable WAV support")

FetchContent_Declare(
    SDL2_mixer
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
    GIT_TAG        release-2.6.3
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(SDL2_mixer)
target_include_directories(yoyoengine PRIVATE ${SDL2_mixer_SOURCE_DIR}/include)

###############
#     lua     #
###############

# NOTE: linked as static per the reccomendation of the cmake port maintainer

FetchContent_Declare(
    lua
    GIT_REPOSITORY https://github.com/walterschell/Lua.git
    GIT_TAG        v5.4.5
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(lua)
target_include_directories(yoyoengine PRIVATE ${lua_SOURCE_DIR}/include)

###############
#   linking   #
###############

target_link_libraries(yoyoengine PRIVATE jansson zlibstatic SDL2-static SDL2_ttf SDL2_image SDL2_mixer lua_static)

# create an output dir based on platform
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_SYSTEM_NAME}/lib)

# set the output directory for the executable
set_target_properties(yoyoengine PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_SYSTEM_NAME}/lib)

###############
#    libs     #
###############

# Set the output directory for libraries
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/${CMAKE_SYSTEM_NAME}/lib)

# Copy libraries to the output directory after build
add_custom_command(
    TARGET yoyoengine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${LIBRARY_OUTPUT_PATH}
    # COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:SDL2-static> ${LIBRARY_OUTPUT_PATH}
    # COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:SDL2_ttf> ${LIBRARY_OUTPUT_PATH}
    # COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:SDL2_image> ${LIBRARY_OUTPUT_PATH}
    # COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:SDL2_mixer> ${LIBRARY_OUTPUT_PATH}
    # COMMENT "Copying libraries to platform-specific directory"
    COMMENT "Creating platform-specific bin directory"
)

###############
#   headers   #
###############

set(INCLUDE_FOLDER ${CMAKE_BINARY_DIR}/bin/${CMAKE_SYSTEM_NAME}/include)
file(MAKE_DIRECTORY ${INCLUDE_FOLDER})

add_custom_target(CopyHeaders
    COMMAND ${CMAKE_COMMAND} -E make_directory ${INCLUDE_FOLDER}/jansson
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${jansson_BINARY_DIR}/include ${INCLUDE_FOLDER}/jansson
    
    COMMAND ${CMAKE_COMMAND} -E make_directory ${INCLUDE_FOLDER}/SDL2
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${SDL2_BINARY_DIR}/include/SDL2 ${INCLUDE_FOLDER}/SDL2
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${SDL2_image_SOURCE_DIR}/include ${INCLUDE_FOLDER}/SDL2
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${SDL2_mixer_SOURCE_DIR}/include ${INCLUDE_FOLDER}/SDL2
    COMMAND ${CMAKE_COMMAND} -E copy ${SDL2_ttf_SOURCE_DIR}/SDL_ttf.h ${INCLUDE_FOLDER}/SDL2
    
    COMMAND ${CMAKE_COMMAND} -E make_directory ${INCLUDE_FOLDER}/lua
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${lua_SOURCE_DIR}/lua-5.4.5/include ${INCLUDE_FOLDER}/lua
    
    COMMAND ${CMAKE_COMMAND} -E make_directory ${INCLUDE_FOLDER}/zlib
    COMMAND ${CMAKE_COMMAND} -E copy ${SDL2_image_SOURCE_DIR}/external/zlib/zlib.h ${INCLUDE_FOLDER}/zlib

    COMMAND ${CMAKE_COMMAND} -E make_directory ${INCLUDE_FOLDER}/yoyoengine
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/dist/include/yoyoengine ${INCLUDE_FOLDER}/yoyoengine

    COMMAND ${CMAKE_COMMAND} -E make_directory ${INCLUDE_FOLDER}/Nuklear
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/dist/include/Nuklear ${INCLUDE_FOLDER}/Nuklear

    COMMAND ${CMAKE_COMMAND} -E make_directory ${INCLUDE_FOLDER}/uthash
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/dist/include/uthash ${INCLUDE_FOLDER}/uthash

    # copy an additional minimal config that for some reason SDL wont generate itself
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/dist/include/SDL2 ${INCLUDE_FOLDER}/SDL2

    COMMENT "Copying header files files to platform-specific directory"
)

# make sure we copy the headers AFTER SDL copies its own headers in its weird little freaky way
add_dependencies(CopyHeaders sdl_headers_copy)
add_dependencies(yoyoengine CopyHeaders)

# we need to include this. This is because when we build the engine we know of all these headers, but from their FetchContent location,
# which causes issues because OUR header files for the engine are written to include from our GENERATED include folder, not the fetchcontent
target_include_directories(yoyoengine PRIVATE ${INCLUDE_FOLDER})

###############
#  resources  #
###############

# configure option
option(YOYO_CMAKE_COPY_ENGINE_RESOURCES "Copy engine resources to the output directory" OFF)

if(YOYO_CMAKE_COPY_ENGINE_RESOURCES)
    add_custom_command(
        TARGET yoyoengine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/${CMAKE_SYSTEM_NAME}/engine_resources
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/dist/assets ${CMAKE_BINARY_DIR}/bin/${CMAKE_SYSTEM_NAME}/engine_resources

        COMMENT "Copying engine resources"
    )
endif()