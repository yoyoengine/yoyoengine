# This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
# Copyright (C) 2024  Ryan Zmuda

# Licensed under the MIT license. See LICENSE file in the project root for details.



###############
#    notes    #
###############

# - all of the headers that the engine links to are PUBLIC, so that the editor and any games automatically get them
# - the engine itself builds deps statically and into one singular shared library
# - the engine copies its resources to the output as well
# - at the end of this file, a shared lib exists alongside a partially populated game build output with the engine resources

###############
#    setup    #
###############

cmake_minimum_required(VERSION 3.22.1)
project(yoyoengine)

file(GLOB_RECURSE SOURCES CMAKE_CONFIGURE_DEPENDS src/*.c)

add_library(yoyoengine SHARED ${SOURCES})

target_include_directories(yoyoengine PUBLIC dist/include/)

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
target_include_directories(yoyoengine PUBLIC ${jansson_BINARY_DIR}/include)

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
target_include_directories(yoyoengine PUBLIC ${SDL2_SOURCE_DIR}/include)

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
target_include_directories(yoyoengine PUBLIC ${zlib_SOURCE_DIR})

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
#   SDL_gfx   #
###############

# NOTE: SDL_gfx is extremely deprecated, and not worth bothering with anymore. I'm leaving this here just in case

# FetchContent_Declare(
#     SDL_gfx
#     GIT_REPOSITORY https://github.com/zoogies/SDL_gfx-cmake
#     GIT_PROGRESS TRUE
# )
# FetchContent_MakeAvailable(SDL_gfx)
# target_include_directories(SDL_gfx PUBLIC ${SDL2_SOURCE_DIR}/include) # give access to SDL.h
# target_include_directories(yoyoengine PUBLIC ${SDL_gfx_SOURCE_DIR}) # include is in the root of the repo

###############
#  SDL2_ttf   #
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
target_include_directories(yoyoengine PUBLIC ${SDL2_ttf_SOURCE_DIR}) # include is in the root of the repo

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
target_include_directories(yoyoengine PUBLIC ${SDL2_image_SOURCE_DIR}/include)

# include the zlib that SDL_image uses - this is for a stray header we are missing
target_include_directories(yoyoengine PUBLIC ${SDL2_image_SOURCE_DIR}/external/zlib)

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
target_include_directories(yoyoengine PUBLIC ${SDL2_mixer_SOURCE_DIR}/include)

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
target_include_directories(yoyoengine PUBLIC ${lua_SOURCE_DIR}/lua-5.4.5/include)

###############
#   SDL2_net  #
###############

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build the library as a shared library")

FetchContent_Declare(
    SDL2_net
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_net.git
    GIT_TAG        release-2.2.0
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(SDL2_net)
target_include_directories(yoyoengine PUBLIC ${SDL2_net_SOURCE_DIR})

###############
#   linking   #
###############

target_link_libraries(yoyoengine PRIVATE
    -Wl,--whole-archive
    jansson
    zlibstatic
    SDL2-static
    SDL2_ttf
    SDL2_image
    SDL2_mixer
    SDL2_net
    lua_static
    -Wl,--no-whole-archive
)

# create an output dir based on platform
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_SYSTEM_NAME}/lib)

# set the output directory for the executable
set_target_properties(yoyoengine PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_SYSTEM_NAME}/lib)

###############
#    libs     #
###############

# Set the output directory for libraries, on linux this is lib, on windows its bin
if(UNIX)
    set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/${CMAKE_SYSTEM_NAME}/lib)
else()
    set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/${CMAKE_SYSTEM_NAME})
endif()

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

# WIP: ability to concat all lua files into one runtime file
# maybe in the future we have this as a seperate target, its
# hard to combine this as a pre-step since we have such a convoluted
# build process (especially for individual games)

###############
# lua runtime #
###############

# Expose the option to build the lua runtime, which the editor will enable.
# The editor creates packs for the games based off of its own engine_resources folder,
# so we will have it build the lua runtime there.

option(BUILD_LUA_RUNTIME "Build the Lua runtime" OFF)

message(STATUS "BUILD_LUA_RUNTIME: ${BUILD_LUA_RUNTIME}")
message(STATUS "LUA_RUNTIME_OUTPUT: ${LUA_RUNTIME_OUTPUT}")

if(BUILD_LUA_RUNTIME)
    
    # where we will output the runtime
    option(LUA_RUNTIME_OUTPUT "Where to output the Lua runtime" ${CMAKE_BINARY_DIR}/bin/${CMAKE_SYSTEM_NAME}/ye_runtime.lua)

    # variable to hold runtime source file location
    set(LUA_RUNTIME_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/lua_runtime
        # ${CMAKE_CURRENT_SOURCE_DIR}/dist/resources/lua_runtime
    )

    # all the files that make up the runtime
    set(LUA_RUNTIME_FILES
        # ${LUA_RUNTIME_SRC}/api.lua
        ${LUA_RUNTIME_SRC}/runtime.lua
        ${LUA_RUNTIME_SRC}/definitions/input_codes.lua
        ${LUA_RUNTIME_SRC}/subsystems/audio.lua
        ${LUA_RUNTIME_SRC}/subsystems/scene.lua
        ${LUA_RUNTIME_SRC}/subsystems/input.lua
        ${LUA_RUNTIME_SRC}/subsystems/timer.lua
        ${LUA_RUNTIME_SRC}/ecs/audiosource.lua
        ${LUA_RUNTIME_SRC}/ecs/button.lua
        ${LUA_RUNTIME_SRC}/ecs/camera.lua
        ${LUA_RUNTIME_SRC}/ecs/collider.lua
        ${LUA_RUNTIME_SRC}/ecs/lua_script.lua
        ${LUA_RUNTIME_SRC}/ecs/physics.lua
        ${LUA_RUNTIME_SRC}/ecs/renderer.lua
        ${LUA_RUNTIME_SRC}/ecs/tag.lua
        ${LUA_RUNTIME_SRC}/ecs/transform.lua
        ${LUA_RUNTIME_SRC}/ecs/entity.lua
    )

    # Concatenate all the runtime source into one file

    # Clear the output file first
    add_custom_command(
        OUTPUT ${LUA_RUNTIME_OUTPUT}
        COMMAND ${CMAKE_COMMAND} -E echo "" > ${LUA_RUNTIME_OUTPUT}
        COMMENT "Clearing Lua runtime output file..."
    )

    # Concatenate each Lua file with a newline after each
    foreach(LUA_FILE ${LUA_RUNTIME_FILES})
    add_custom_command(
        OUTPUT ${LUA_RUNTIME_OUTPUT}
        COMMAND ${CMAKE_COMMAND} -E echo_append "" >> ${LUA_RUNTIME_OUTPUT} # Ensure it starts with a newline (optional)
        COMMAND ${CMAKE_COMMAND} -E cat ${LUA_FILE} >> ${LUA_RUNTIME_OUTPUT}
        COMMAND ${CMAKE_COMMAND} -E echo "" >> ${LUA_RUNTIME_OUTPUT} # Append a newline after the file content
        DEPENDS ${LUA_FILE}
        APPEND
        COMMENT "Appending ${LUA_FILE} and a newline to the runtime output..."
    )
    endforeach()

    # Final step to touch the output file and print a message
    add_custom_command(
        OUTPUT ${LUA_RUNTIME_OUTPUT}
        COMMAND ${CMAKE_COMMAND} -E touch ${LUA_RUNTIME_OUTPUT}
        COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --blue "Finished concatenating Lua files into ${LUA_RUNTIME_OUTPUT}"
        APPEND
        COMMENT "Finalizing Lua runtime concatenation..."
    )

    # make our engine depend on this command
    add_custom_target(lua_runtime ALL DEPENDS ${LUA_RUNTIME_OUTPUT})
endif()

###############
#  resources  #
###############

# configure option
option(YOYO_CMAKE_COPY_ENGINE_RESOURCES "Copy engine resources to the output directory" OFF)

if(YOYO_CMAKE_COPY_ENGINE_RESOURCES)
    add_custom_command(
        TARGET yoyoengine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/${CMAKE_SYSTEM_NAME}/engine_resources
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/dist/resources ${CMAKE_BINARY_DIR}/bin/${CMAKE_SYSTEM_NAME}/engine_resources

        COMMENT "Copying engine resources"
    )
endif()

###############
#   license   #
###############

if(NOT EXISTS ${CMAKE_BINARY_DIR}/bin/${CMAKE_SYSTEM_NAME}/LICENSE.md)
    add_custom_command(
        TARGET yoyoengine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/../LICENSE.md ${CMAKE_BINARY_DIR}/bin/${CMAKE_SYSTEM_NAME}/LICENSE.md
        COMMENT "Copying license to build directory"
    )
endif()