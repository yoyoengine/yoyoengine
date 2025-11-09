# This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
# Copyright (C) 2023-2025  Ryan Zmuda

# Licensed under the MIT license. See LICENSE file in the project root for details.



###############
#    setup    #
###############

cmake_minimum_required(VERSION 3.22.1)
project(yoyoengine)

file(GLOB_RECURSE SOURCES CMAKE_CONFIGURE_DEPENDS src/*.c)

if(MSVC)
    # set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    # # MSVC sucks ^

    set(YOYO_ENGINE_STATIC ON CACHE BOOL "Building YOYOENGINE statically")
    set(YOYO_ENGINE_INTERFACE OFF CACHE BOOL "YOYOENGINE set as an interface target")
endif()

option(YOYO_ENGINE_STATIC "Build YOYOENGINE as a static library" OFF)
option(YOYO_ENGINE_INTERFACE "Configure YOYOENGINE as an interface target" OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

if(YOYO_ENGINE_INTERFACE)
    message(STATUS "Configuring yoyoengine as an interface target")
    add_library(yoyoengine INTERFACE ${SOURCES})
    set(YOYO_ENGINE_LINK_TERM "INTERFACE" CACHE STRING "The link type for yoyoengine")
else()
    if(YOYO_ENGINE_STATIC)
        message(STATUS "Configuring yoyoengine as a static library")
        add_library(yoyoengine STATIC ${SOURCES})
        set(YOYO_ENGINE_LINK_TERM "PUBLIC" CACHE STRING "The link type for yoyoengine")
    else()
        message(STATUS "Configuring yoyoengine as a shared library")
        add_library(yoyoengine SHARED ${SOURCES})

        # required (originated from p2d) but is allegedly (claude) standard for shared libs
        set(CMAKE_POSITION_INDEPENDENT_CODE ON)

        set(YOYO_ENGINE_LINK_TERM "PUBLIC" CACHE STRING "The link type for yoyoengine")
    endif()
endif()

# TODO: right now this would only be applicable on a shared build on windows, which doesn't exist
# target_compile_definitions(yoyoengine PRIVATE YOYO_ENGINE_BUILDING)
# TODO: how does this fit in with declspec? ^

set_target_properties(yoyoengine PROPERTIES LINKER_LANGUAGE C)
target_include_directories(yoyoengine PUBLIC dist/include/)

include(FetchContent)
Set(FETCHCONTENT_QUIET FALSE)

# if not in release mode, build in debug mode which sets cflags for max warnings and debug sigs
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(MSVC)
        # TODO: make these proper
        # target_compile_options(yoyoengine PRIVATE -Wall -g)
    else()
        target_compile_options(yoyoengine PRIVATE -Wall -Wextra -g)
    endif()
    message(STATUS "Building yoyoengine in debug mode")
else()
    # target_compile_options(yoyoengine PRIVATE -03) COMPILER OPTIMIZATION DESTROYS ANIMATION SYSTEM
    target_link_options(yoyoengine PRIVATE -s)
    message(STATUS "Building yoyoengine in release mode")
endif()

# if ZOOGIES_DEVELOPMENT_BUILD, every warning should fail
option(ZOOGIES_DEVELOPMENT_BUILD "Enable all warnings" OFF)
if(ZOOGIES_DEVELOPMENT_BUILD)
    if(MSVC)
        # TODO: make these proper
        # target_compile_options(yoyoengine PRIVATE)
    else()
        target_compile_options(yoyoengine PRIVATE -Werror -Wall -Wextra)
    endif()
    message(STATUS "Building yoyoengine with maximum warnings enabled")

    # Enable debug information
    set(CMAKE_BUILD_TYPE Debug)
endif()

###############
#   Lilith    #
###############

# TODO: pin tag when stable
FetchContent_Declare(
    lilith
    GIT_REPOSITORY https://github.com/yoyoengine/lilith.git
    GIT_TAG main
    GIT_PROGRESS TRUE
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(lilith)
target_include_directories(yoyoengine PUBLIC ${lilith_SOURCE_DIR}/include) # Lilith.h

###############
#     p2d     #
###############

# TODO: pin tag when stable
FetchContent_Declare(
    p2d
    GIT_REPOSITORY https://github.com/yoyoengine/p2d.git
    GIT_TAG main
    GIT_PROGRESS TRUE
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(p2d)
target_include_directories(yoyoengine PUBLIC ${p2d_SOURCE_DIR}/include)

###############
#   Nuklear   #
###############

# going to use custom branch ref so we dont have to add a populate command to `make nuke`
FetchContent_Declare(
    nuklear
    GIT_REPOSITORY https://github.com/yoyoengine/Nuklear.git
    GIT_TAG yoyoengine # WIP SDL3 Nuklear port
    GIT_PROGRESS TRUE
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(nuklear)
target_include_directories(yoyoengine PUBLIC ${nuklear_SOURCE_DIR})
target_include_directories(yoyoengine PUBLIC ${nuklear_SOURCE_DIR}/demo/sdl3)

###############
#   jansson   #
###############

# Build as shared and without docs - NOTE: jansson doesnt give af about the shared param and builds static anyways
# set(BUILD_SHARED 1 CACHE INTERNAL "")
# dont build tests
set(JANSSON_WITHOUT_TESTS ON CACHE BOOL "Disable building of tests")
set(JANSSON_BUILD_DOCS OFF CACHE INTERNAL "")
set(JANSSON_BUILD_MAN OFF CACHE INTERNAL "")
FetchContent_Declare(
    jansson
    GIT_REPOSITORY https://github.com/akheron/jansson.git
    GIT_TAG        v2.14.1
    GIT_PROGRESS TRUE
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(jansson)
target_include_directories(yoyoengine PUBLIC ${jansson_BINARY_DIR}/include)

###############
#    SDL3     #
###############

set(SDL3_DISABLE_INSTALL OFF CACHE BOOL "Disable installation of SDL3") # i forget why this is necessary but im assuming it wont work without
set(SDL_STATIC ON CACHE BOOL "Build a static library")
set(SDL_SHARED OFF CACHE BOOL "Build a shared library")
set(SDL_TEST OFF CACHE BOOL "Build the test programs")
set(SDL_STATIC_PIC ON CACHE BOOL "Build static libraries with PIC (Position Independent Code)")
FetchContent_Declare(
    SDL3
    GIT_REPOSITORY  https://github.com/libsdl-org/SDL.git
    GIT_TAG         release-3.2.8
    GIT_PROGRESS    TRUE
    GIT_SHALLOW     TRUE
)
# set(SDL_SHARED OFF CACHE BOOL "Build a shared library")
FetchContent_MakeAvailable(SDL3)
target_include_directories(yoyoengine PUBLIC ${SDL3_SOURCE_DIR}/include/) # this one is for editor
target_include_directories(yoyoengine PUBLIC ${SDL3_SOURCE_DIR}/include/SDL3)
# if(MSVC)
#     target_link_libraries(yoyoengine PUBLIC SDL3-static)
#     # target_link_libraries(yoyoengine PUBLIC SDL3main)
# endif()

###############
#    zlib     #
###############

# set(ZLIB_USE_STATIC_LIBS ON CACHE BOOL "Use static libraries for zlib")
# set(ZLIB_USE_SHARED_LIBS OFF CACHE BOOL "Use shared libraries for zlib")

set(ZLIB_BUILD_EXAMPLES OFF CACHE BOOL "Disable building of zlib examples")

FetchContent_Declare(
    zlib
    GIT_REPOSITORY https://github.com/madler/zlib.git
    GIT_TAG        v1.3.1
    GIT_PROGRESS TRUE
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(zlib)
target_include_directories(yoyoengine PUBLIC ${zlib_SOURCE_DIR})

################
#   SDL3_ttf   #
################

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build the library as a shared library")
# set(SDL2TTF_BUILD_SHARED_LIBS OFF CACHE BOOL "Build the library as a shared library")
set(SDLTTF_SAMPLES OFF CACHE BOOL "Build the SDL3_ttf sample program(s)")
# let it know it needs to get its vendored deps
set(SDLTTF_VENDORED  ON CACHE BOOL "Use the vendored version of FreeType and HarfBuzz")

# force requiring zlib and override anything else that might try to change that
# set(FT_REQUIRE_ZLIB TRUE CACHE INTERNAL "" FORCE)
# set(FT_DISABLE_ZLIB FALSE CACHE INTERNAL "" FORCE)

# set(SDL2TTF_INSTALL OFF CACHE BOOL "Generate installation target")

FetchContent_Declare(
    SDL3_ttf
    GIT_REPOSITORY  https://github.com/libsdl-org/SDL_ttf
    GIT_TAG         release-3.2.2
    GIT_PROGRESS    TRUE
    GIT_SHALLOW     TRUE
)
FetchContent_MakeAvailable(SDL3_ttf)

target_include_directories(yoyoengine PUBLIC ${SDL3_ttf_SOURCE_DIR}/include/SDL3_ttf) # include is in the root of the repo

################
#  SDL3_image  #
################

set(SDL3IMAGE_DEPS_SHARED OFF CACHE BOOL "Build the dependencies as shared libraries")

# set(PNG_SHARED OFF CACHE BOOL "Build a shared version of the library")

# set(PNG_BUILD_ZLIB ON CACHE BOOL "Build zlib")

set(SDLIMAGE_PNG ON CACHE BOOL "Support loading PNG images")
set(SDLIMAGE_JPG ON CACHE BOOL "Support loading JPEG images")

set(SDLIMAGE_AVIF_VENDORED OFF CACHE BOOL "Use the vendored version of libavif")

set(SDLIMAGE_AVIF OFF CACHE BOOL  "Support loading AVIF images")
set(SDLIMAGE_BMP OFF CACHE BOOL "Support loading BMP images")
set(SDLIMAGE_GIF OFF CACHE BOOL "Support loading GIF images")
set(SDLIMAGE_JXL OFF CACHE BOOL "Support loading JXL images" )
set(SDLIMAGE_LBM OFF CACHE BOOL "Support loading LBM images")
set(SDLIMAGE_PCX OFF CACHE BOOL "Support loading PCX images")
set(SDLIMAGE_PNM OFF CACHE BOOL "Support loading PNM images")
set(SDLIMAGE_QOI OFF CACHE BOOL "Support loading QOI images")
set(SDLIMAGE_SVG OFF CACHE BOOL "Support loading SVG images")
set(SDLIMAGE_TGA OFF CACHE BOOL "Support loading TGA images")
set(SDLIMAGE_TIF OFF CACHE BOOL "Support loading TIFF images")
set(SDLIMAGE_WEBP OFF CACHE BOOL  "Support loading WEBP images")
set(SDLIMAGE_XCF OFF CACHE BOOL "Support loading XCF images")
set(SDLIMAGE_XPM OFF CACHE BOOL "Support loading XPM images")
set(SDLIMAGE_XV OFF CACHE BOOL "Support loading XV images")

FetchContent_Declare(
    SDL3_image
    GIT_REPOSITORY  https://github.com/libsdl-org/SDL_image.git
    GIT_TAG         release-3.2.4
    GIT_PROGRESS    TRUE
    GIT_SHALLOW     TRUE
)
FetchContent_MakeAvailable(SDL3_image)
target_include_directories(yoyoengine PUBLIC ${SDL3_image_SOURCE_DIR}/include/SDL3_image)

# include the zlib that SDL_image uses - this is for a stray header we are missing
target_include_directories(yoyoengine PUBLIC ${SDL3_image_SOURCE_DIR}/external/zlib)

################
#  SDL3_mixer  #
################

# disable annoying codecs we dont need that require dependencies i dont have
set(SDLMIXER_OPUS  OFF CACHE BOOL "Enable Opus support")
set(SDLMIXER_FLAC  OFF CACHE BOOL "Enable FLAC support")
set(SDLMIXER_MOD   OFF CACHE BOOL "Enable Mod support")
set(SDLMIXER_MIDI  OFF CACHE BOOL "Enable MIDI support")

# mp3 enabled
set(SDLMIXER_MP3 ON CACHE BOOL "Enable MP3 support")

# wav enabled
set(SDLMIXER_WAV ON CACHE BOOL "Enable WAV support")

FetchContent_Declare(
    SDL3_mixer
    GIT_REPOSITORY  https://github.com/libsdl-org/SDL_mixer.git
    GIT_TAG         05099c5ff7493f80a90b1fd18103697715f412d0  # TODO: update when SDL3; random version that works
    GIT_PROGRESS TRUE
    GIT_SHALLOW FALSE
)
FetchContent_MakeAvailable(SDL3_mixer)
target_include_directories(yoyoengine PUBLIC ${SDL3_mixer_SOURCE_DIR}/include/SDL3_mixer)

###############
#     lua     #
###############

# NOTE: linked as static per the reccomendation of the cmake port maintainer

FetchContent_Declare(
    lua
    GIT_REPOSITORY https://github.com/walterschell/Lua.git
    GIT_TAG        v5.4.5
    GIT_PROGRESS TRUE
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(lua)
target_include_directories(yoyoengine PUBLIC ${lua_SOURCE_DIR}/lua-5.4.5/include)

# ###############
# #   SDL3_net  #
# ###############

# set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build the library as a shared library")

# FetchContent_Declare(
#     SDL3_net
#     GIT_REPOSITORY  https://github.com/libsdl-org/SDL_net.git
#     GIT_TAG         7de8a144c3d2322a755ec881976bcdc7e882e03c # TODO: SDL3 pin when ready
#     GIT_PROGRESS    TRUE
#     GIT_SHALLOW     TRUE
# )
# FetchContent_MakeAvailable(SDL3_net)
# target_include_directories(yoyoengine PUBLIC ${SDL3_net_SOURCE_DIR}/include/SDL3_net)

###############
#   linking   #
###############

if(YOYO_ENGINE_STATIC)
    target_link_libraries(yoyoengine ${YOYO_ENGINE_LINK_TERM} 
        # TODO: is there lilith duplication in p2d?
        lilith
        p2d
        jansson
        zlibstatic
        SDL3-static
        SDL3_ttf-static
        SDL3_image-static
        SDL3_mixer-static
        # SDL3_net-static
        lua_static
    )
else()
    # On GCC/Clang, use whole-archive to ensure all symbols are included in shared lib
    target_link_libraries(yoyoengine ${YOYO_ENGINE_LINK_TERM}  
        -Wl,--whole-archive
        # TODO: is there lilith duplication in p2d?
        lilith
        p2d
        jansson
        zlibstatic
        SDL3-static
        SDL3_ttf-static
        SDL3_image-static
        SDL3_mixer-static
        # SDL3_net-static
        lua_static
        -Wl,--no-whole-archive
    )
endif()

# ###############
# # lua runtime #
# ###############

# # Expose the option to build the lua runtime, which the editor will enable.
# # The editor creates packs for the games based off of its own engine_resources folder,
# # so we will have it build the lua runtime there.

# option(BUILD_LUA_RUNTIME "Build the Lua runtime" OFF)

# message(STATUS "BUILD_LUA_RUNTIME: ${BUILD_LUA_RUNTIME}")

# if(BUILD_LUA_RUNTIME)
    
#     # where we will output the runtime
#     option(LUA_RUNTIME_OUTPUT "Where to output the Lua runtime" "${CMAKE_BINARY_DIR}/$<CONFIGURATION>/ye_runtime.lua")
#     message(STATUS "LUA_RUNTIME_OUTPUT: ${LUA_RUNTIME_OUTPUT}")

#     # create full path to runtime output so it exists
#     get_filename_component(LUA_RUNTIME_OUTPUT_DIR ${LUA_RUNTIME_OUTPUT} DIRECTORY)
#     file(MAKE_DIRECTORY ${LUA_RUNTIME_OUTPUT_DIR})

#     # variable to hold runtime source file location
#     set(LUA_RUNTIME_SRC
#         ${CMAKE_CURRENT_SOURCE_DIR}/src/lua_runtime
#         # ${CMAKE_CURRENT_SOURCE_DIR}/dist/resources/lua_runtime
#     )

#     # all the files that make up the runtime
#     set(LUA_RUNTIME_FILES
#         # ${LUA_RUNTIME_SRC}/api.lua
#         ${LUA_RUNTIME_SRC}/runtime.lua
#         ${LUA_RUNTIME_SRC}/definitions/input_codes.lua
#         ${LUA_RUNTIME_SRC}/subsystems/audio.lua
#         ${LUA_RUNTIME_SRC}/subsystems/scene.lua
#         ${LUA_RUNTIME_SRC}/subsystems/input.lua
#         ${LUA_RUNTIME_SRC}/subsystems/timer.lua
#         ${LUA_RUNTIME_SRC}/ecs/audiosource.lua
#         ${LUA_RUNTIME_SRC}/ecs/button.lua
#         ${LUA_RUNTIME_SRC}/ecs/camera.lua
#         ${LUA_RUNTIME_SRC}/ecs/collider.lua
#         ${LUA_RUNTIME_SRC}/ecs/lua_script.lua
#         ${LUA_RUNTIME_SRC}/ecs/physics.lua
#         ${LUA_RUNTIME_SRC}/ecs/renderer.lua
#         ${LUA_RUNTIME_SRC}/ecs/tag.lua
#         ${LUA_RUNTIME_SRC}/ecs/transform.lua
#         ${LUA_RUNTIME_SRC}/ecs/entity.lua
#     )

#     # Concatenate all the runtime source into one file

#     # Clear the output file first
#     add_custom_command(
#         OUTPUT ${LUA_RUNTIME_OUTPUT}
#         COMMAND ${CMAKE_COMMAND} -E echo "" > ${LUA_RUNTIME_OUTPUT}
#         COMMENT "Clearing Lua runtime output file..."
#     )

#     # Concatenate each Lua file with a newline after each
#     foreach(LUA_FILE ${LUA_RUNTIME_FILES})
#     add_custom_command(
#         OUTPUT ${LUA_RUNTIME_OUTPUT}
#         COMMAND ${CMAKE_COMMAND} -E echo_append "" >> ${LUA_RUNTIME_OUTPUT} # Ensure it starts with a newline (optional)
#         COMMAND ${CMAKE_COMMAND} -E cat ${LUA_FILE} >> ${LUA_RUNTIME_OUTPUT}
#         COMMAND ${CMAKE_COMMAND} -E echo "" >> ${LUA_RUNTIME_OUTPUT} # Append a newline after the file content
#         DEPENDS ${LUA_FILE}
#         APPEND
#         COMMENT "Appending ${LUA_FILE} and a newline to the runtime output..."
#     )
#     endforeach()

#     # Final step to touch the output file and print a message
#     add_custom_command(
#         OUTPUT ${LUA_RUNTIME_OUTPUT}
#         COMMAND ${CMAKE_COMMAND} -E touch ${LUA_RUNTIME_OUTPUT}
#         COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --blue "Finished concatenating Lua files into ${LUA_RUNTIME_OUTPUT}"
#         APPEND
#         COMMENT "Finalizing Lua runtime concatenation..."
#     )

#     # make our engine depend on this command
#     add_custom_target(lua_runtime ALL DEPENDS ${LUA_RUNTIME_OUTPUT})
# endif()

# TODO: these two dont not work for INTERFACE

###############
#  resources  #
###############

# configure option
# option(YOYO_CMAKE_COPY_ENGINE_RESOURCES "Copy engine resources to the output directory" OFF)

option(YOYO_ENGINE_COPY_ENGINE_RESOURCES "Copy engine resources to the output directory" OFF)

# TODO: this should just be pack files at some point
# TODO: this doesn't account for "old" engine resources
if(YOYO_ENGINE_COPY_ENGINE_RESOURCES AND NOT EXISTS ${CMAKE_BINARY_DIR}/$<CONFIGURATION>/engine_resources)
    add_custom_command(
        TARGET yoyoengine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/$<CONFIGURATION>/engine_resources
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/dist/resources ${CMAKE_BINARY_DIR}/$<CONFIGURATION>/engine_resources
        COMMENT "Copying engine resources"
    )
endif()

###############
#   license   #
###############

if(NOT EXISTS ${CMAKE_BINARY_DIR}/$<CONFIGURATION>/LICENSE.md)
    add_custom_command(
        TARGET yoyoengine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/../LICENSE.md ${CMAKE_BINARY_DIR}/$<CONFIGURATION>/LICENSE.md
        COMMENT "Copying license to build directory"
    )
endif()